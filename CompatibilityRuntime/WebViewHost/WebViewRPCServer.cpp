#include "WebViewRPCServer.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <poll.h>

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_callback.h>
#include <include/cef_app.h>

#include <WebView/WebViewProtocol.pb.h>
#include <WebViewProtocolController.h>

#include <cstring>

WebViewRPCServer::WebViewRPCServer(intptr_t connectedSocket) : m_socket(connectedSocket) {
    printf("WebViewRPCServer: working over %d\n", m_socket);

    fcntl(m_socket, F_SETFL, fcntl(m_socket, F_GETFL) | O_NONBLOCK);
    fcntl(m_socket, F_SETFD, fcntl(m_socket, F_GETFD) | FD_CLOEXEC);
    m_requestReceivingThread = std::thread(&WebViewRPCServer::receiveRequests, this);
}

WebViewRPCServer::~WebViewRPCServer() {
    shutdown(m_socket, SHUT_RDWR);
    m_requestReceivingThread.join();
}

void WebViewRPCServer::receiveRequests() {
    printf("WebViewRPCServer: receiving requests\n");

    runMainLoop();

    printf("WebViewRPCServer: main loop exited, signaling the browser UI thread to stop\n");

    CefPostTask(TID_UI, base::BindOnce(CefQuitMessageLoop));
}

void WebViewRPCServer::runMainLoop() {
    while(true) {
        uint32_t messageLength;
        if(!ensuredRead(&messageLength, sizeof(messageLength)))
            return;

        if(m_receiveBuffer.size() < messageLength)
            m_receiveBuffer.resize(messageLength);

        if(!ensuredRead(m_receiveBuffer.data(), messageLength))
            return;

        auto fullRequest = std::make_unique<webview::protocol::RPCRequest>();
        if(!fullRequest->ParseFromArray(m_receiveBuffer.data(), messageLength)) {
            fprintf(stderr, "WebViewRPCServer: failed to parse the RPC request\n");
            return;
        }

        CefPostTask(TID_UI, base::BindOnce(&WebViewRPCServer::executeRPCCall, base::Unretained(this), std::move(fullRequest)));
    }
}

void WebViewRPCServer::executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    if(!doExecuteRPCCall(std::move(request))) {
        fprintf(stderr, "WebViewRPCServer: an error occurred during RPC processing\n");

        CefQuitMessageLoop();
    }
}

bool WebViewRPCServer::doExecuteRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&fullRequest) {
    auto method = m_service.GetDescriptor()->FindMethodByName(fullRequest->method());
    if(!method) {
        fprintf(stderr, "WebViewRPCServer: nonexistent RPC method '%s' called\n",
                fullRequest->method().c_str());
        return false;
    }

    std::unique_ptr<google::protobuf::Message> request(m_service.GetRequestPrototype(method).New());
    std::unique_ptr<google::protobuf::Message> response(m_service.GetResponsePrototype(method).New());

    if(!request->ParseFromString(fullRequest->request())) {
        fprintf(stderr, "WebViewRPCServer: failed to parse the RPC request body\n");
        return false;
    }

    WebViewProtocolController controller;
    m_service.CallMethod(method, &controller, request.get(), response.get(), nullptr);

    webview::protocol::RPCResponse fullResponse;
    if(controller.Failed()) {
        fullResponse.set_errormessage(controller.ErrorText());
    } else {
        fullResponse.set_response(response->SerializeAsString());
    }

    auto fullResponseData = fullResponse.SerializeAsString();

    uint32_t messageLength = fullResponseData.size();
    if(!ensuredWrite(&messageLength, sizeof(messageLength)))
        return false;

    if(!ensuredWrite(fullResponseData.data(), fullResponseData.size()))
        return false;

    return true;
}

bool WebViewRPCServer::ensuredRead(void *dest, size_t size) {
    if(size == 0)
        return true;

    auto bytes = static_cast<unsigned char *>(dest);
    while(size > 0) {
        ssize_t result;
        do {
            result = recv(m_socket, bytes, size, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            if(errno == EWOULDBLOCK) {
                if(!pollFor(POLLIN))
                    return false;

                continue;
            } else {
                fprintf(stderr, "WebViewRPCServer::ensuredRead: recv signaled %s\n", strerror(errno));
                return false;
            }
        } else if(result == 0) {
            fprintf(stderr, "WebViewRPCServer::ensuredRead: end of file\n");
            return false;
        }

        bytes += result;
        size -= result;
    }

    return true;
}
bool WebViewRPCServer::ensuredWrite(const void *dest, size_t size) {
    if(size == 0)
        return true;

    auto bytes = static_cast<const unsigned char *>(dest);
    while(size > 0) {
        ssize_t result;
        do {
            result = send(m_socket, bytes, size, 0);
        } while(result < 0 && errno == EINTR);

        if(result < 0) {
            if(errno == EWOULDBLOCK) {
                if(!pollFor(POLLOUT))
                    return false;

                continue;
            } else {
                fprintf(stderr, "WebViewRPCServer::ensuredWrite: send signaled %s\n", strerror(errno));
                return false;
            }
        }

        bytes += result;
        size -= result;
    }

    return true;
}

bool WebViewRPCServer::pollFor(int condition) {
    struct pollfd pfd;
    pfd.fd = m_socket;
    pfd.events = condition;

    int result;
    do {
        do {
            result = poll(&pfd, 1, -1);
        } while(result < 0 && errno == EINTR);

        if(result < 0)
            return false;

        if(pfd.revents & (POLLERR | POLLNVAL))
            return false;
    } while(result == 0 || !(pfd.revents & condition));

    return true;
}


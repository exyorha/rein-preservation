#include "WebViewRPCServerLinux.h"

#include <fcntl.h>

#include <sys/socket.h>
#include <poll.h>

#include <include/wrapper/cef_closure_task.h>
#include <include/base/cef_callback.h>
#include <include/cef_app.h>

WebViewRPCServerLinux::WebViewRPCServerLinux(intptr_t connectedSocket) : m_socket(connectedSocket) {
    printf("WebViewRPCServerLinux: working over %d\n", m_socket);

    fcntl(m_socket, F_SETFL, fcntl(m_socket, F_GETFL) | O_NONBLOCK);
    fcntl(m_socket, F_SETFD, fcntl(m_socket, F_GETFD) | FD_CLOEXEC);
    m_requestReceivingThread = std::thread(&WebViewRPCServerLinux::receiveRequests, this);
}

WebViewRPCServerLinux::~WebViewRPCServerLinux() {
    shutdown(m_socket, SHUT_RDWR);
    m_requestReceivingThread.join();
}

void WebViewRPCServerLinux::receiveRequests() {
    printf("WebViewRPCServerLinux: receiving requests\n");

    runMainLoop();

    printf("WebViewRPCServerLinux: main loop exited, signaling the browser UI thread to stop\n");

    CefPostTask(TID_UI, base::BindOnce(CefQuitMessageLoop));
}

void WebViewRPCServerLinux::executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    if(!executeRPCCallAndSendResponse(std::move(request))) {

        fprintf(stderr, "WebViewRPCServerLinux: an error occurred during RPC processing\n");

        CefQuitMessageLoop();
    }
}

bool WebViewRPCServerLinux::executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCRequest> &&request) {
    auto result = doExecuteRPCCall(std::move(request));
    if(!result.has_value()) {
        return false;
    }

    const auto &fullResponseData = *result;

    uint32_t messageLength = fullResponseData.size();
    if(!ensuredWrite(&messageLength, sizeof(messageLength)))
        return false;

    if(!ensuredWrite(fullResponseData.data(), fullResponseData.size()))
        return false;

    return true;
}

void WebViewRPCServerLinux::runMainLoop() {
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

        CefPostTask(TID_UI, base::BindOnce(&WebViewRPCServerLinux::executeRPCCall, base::Unretained(this), std::move(fullRequest)));
    }
}


bool WebViewRPCServerLinux::ensuredRead(void *dest, size_t size) {
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

bool WebViewRPCServerLinux::ensuredWrite(const void *dest, size_t size) {
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

bool WebViewRPCServerLinux::pollFor(int condition) {
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


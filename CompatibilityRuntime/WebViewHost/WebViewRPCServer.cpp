#include "WebViewRPCServer.h"

#include <WebView/WebViewProtocol.pb.h>
#include <WebViewProtocolController.h>

#include <cstring>

WebViewRPCServer::WebViewRPCServer() : m_callInProgress(false) {

}


WebViewRPCServer::~WebViewRPCServer() = default;

void WebViewRPCServer::clearCallInProgress() {
    {
        std::unique_lock<std::mutex> locker(m_callInProgressMutex);
        m_callInProgress = false;
    }

    m_callInProgressCondvar.notify_all();
}

void WebViewRPCServer::waitNoCallInProgress() {

    std::unique_lock<std::mutex> locker(m_callInProgressMutex);

    m_callInProgressCondvar.wait(locker, [this]() { return !m_callInProgress; });
}

void WebViewRPCServer::setCallInProgress() {
    std::unique_lock<std::mutex> locker(m_callInProgressMutex);
    m_callInProgress = true;
}

std::optional<std::string> WebViewRPCServer::doExecuteRPCCall(
    std::unique_ptr<webview::protocol::RPCMessage> &&message) {

    const auto &fullRequest = message->callrequest();

    auto method = m_service.GetDescriptor()->FindMethodByName(fullRequest.method());
    if(!method) {
        fprintf(stderr, "WebViewRPCServer: nonexistent RPC method '%s' called\n",
                fullRequest.method().c_str());
        return std::nullopt;
    }

    std::unique_ptr<google::protobuf::Message> request(m_service.GetRequestPrototype(method).New());
    std::unique_ptr<google::protobuf::Message> response(m_service.GetResponsePrototype(method).New());

    if(!request->ParseFromString(fullRequest.request())) {
        fprintf(stderr, "WebViewRPCServer: failed to parse the RPC request body\n");
        return std::nullopt;
    }

    WebViewProtocolController controller(this);
    m_service.CallMethod(method, &controller, request.get(), response.get(), nullptr);

    webview::protocol::RPCResponse fullResponse;
    if(controller.Failed()) {
        fullResponse.set_errormessage(controller.ErrorText());
    } else {
        fullResponse.set_response(response->SerializeAsString());
    }

    return fullResponse.SerializeAsString();
}

void WebViewRPCServer::handleNonCallMessage(std::unique_ptr<webview::protocol::RPCMessage> &&request) {
    m_service.handleNonCallMessage(std::move(request));
}

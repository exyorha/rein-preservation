#include "WebViewRPCServer.h"

#include <WebView/WebViewProtocol.pb.h>
#include <WebViewProtocolController.h>

#include <cstring>

WebViewRPCServer::WebViewRPCServer() = default;


WebViewRPCServer::~WebViewRPCServer() = default;



std::optional<std::string> WebViewRPCServer::doExecuteRPCCall(
    std::unique_ptr<webview::protocol::RPCRequest> &&fullRequest) {
    auto method = m_service.GetDescriptor()->FindMethodByName(fullRequest->method());
    if(!method) {
        fprintf(stderr, "WebViewRPCServer: nonexistent RPC method '%s' called\n",
                fullRequest->method().c_str());
        return std::nullopt;
    }

    std::unique_ptr<google::protobuf::Message> request(m_service.GetRequestPrototype(method).New());
    std::unique_ptr<google::protobuf::Message> response(m_service.GetResponsePrototype(method).New());

    if(!request->ParseFromString(fullRequest->request())) {
        fprintf(stderr, "WebViewRPCServer: failed to parse the RPC request body\n");
        return std::nullopt;
    }

    WebViewProtocolController controller;
    m_service.CallMethod(method, &controller, request.get(), response.get(), nullptr);

    webview::protocol::RPCResponse fullResponse;
    if(controller.Failed()) {
        fullResponse.set_errormessage(controller.ErrorText());
    } else {
        fullResponse.set_response(response->SerializeAsString());
    }

    return fullResponse.SerializeAsString();
}


#ifndef WEB_VIEW_RPC_SERVER_H
#define WEB_VIEW_RPC_SERVER_H

#include "WebViewRPCService.h"

class WebViewRPCServer {
protected:
    WebViewRPCServer();
    ~WebViewRPCServer();

public:
    WebViewRPCServer(const WebViewRPCServer &other) = delete;
    WebViewRPCServer &operator =(const WebViewRPCServer &other) = delete;

protected:
    std::optional<std::string> doExecuteRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request);

private:
    WebViewRPCService m_service;
};

#endif

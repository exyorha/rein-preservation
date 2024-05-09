#ifndef WEB_VIEW_RPC_SERVER_H
#define WEB_VIEW_RPC_SERVER_H

#include "WebViewRPCService.h"

#include <mutex>
#include <condition_variable>

class WebViewSharedImageBuffer;

class WebViewRPCServer {
protected:
    WebViewRPCServer();
    ~WebViewRPCServer();

public:
    WebViewRPCServer(const WebViewRPCServer &other) = delete;
    WebViewRPCServer &operator =(const WebViewRPCServer &other) = delete;

    virtual std::unique_ptr<WebViewSharedImageBuffer> receiveImageBuffer(intptr_t handle) = 0;

protected:
    std::optional<std::string> doExecuteRPCCall(std::unique_ptr<webview::protocol::RPCMessage> &&request);
    void handleNonCallMessage(std::unique_ptr<webview::protocol::RPCMessage> &&request);

    void clearCallInProgress();
    void waitNoCallInProgress();
    void setCallInProgress();

private:
    WebViewRPCService m_service;
    bool m_callInProgress;
    std::mutex m_callInProgressMutex;
    std::condition_variable m_callInProgressCondvar;
};

#endif

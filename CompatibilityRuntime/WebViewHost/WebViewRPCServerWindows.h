#ifndef WEB_VIEW_RPC_SERVER_WINDOWS_H
#define WEB_VIEW_RPC_SERVER_WINDOWS_H

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <windows.h>

#include "WebViewRPCServer.h"

class WebViewRPCServerWindows final: public WebViewRPCServer {
public:
    explicit WebViewRPCServerWindows(const std::wstring &server);
    ~WebViewRPCServerWindows();

private:
    void receiveRequests();

    void runMainLoop();

    void executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request);
    bool executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCRequest> &&request);

    void clearCallInProgress();

    std::mutex m_callInProgressMutex;
    std::condition_variable m_callInProgressCondvar;
    bool m_callInProgress;
    std::vector<unsigned char> m_receiveBuffer;
    HANDLE m_handle;
    std::thread m_requestReceivingThread;
};

#endif


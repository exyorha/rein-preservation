#ifndef WEB_VIEW_RPC_SERVER_WINDOWS_H
#define WEB_VIEW_RPC_SERVER_WINDOWS_H

#include <string>
#include <thread>

#include <windows.h>

#include "WebView/WebViewProtocol.pb.h"
#include "WebViewRPCServer.h"
#include "WebViewSharedImageBuffer.h"

class WebViewRPCServerWindows final: public WebViewRPCServer {
public:
    explicit WebViewRPCServerWindows(const std::wstring &server);
    ~WebViewRPCServerWindows();

    std::unique_ptr<WebViewSharedImageBuffer> receiveImageBuffer(intptr_t handle) override;

private:
    class WindowsSharedImageBuffer final : public WebViewSharedImageBuffer {
    public:
        explicit WindowsSharedImageBuffer(HANDLE handle);
        ~WindowsSharedImageBuffer() override;

        void *base() const override;
        size_t size() const override;

    private:
        void *m_base;
        size_t m_size;
    };

    void receiveRequests();

    void runMainLoop();

    void executeRPCCall(std::unique_ptr<webview::protocol::RPCMessage> &&request);
    bool executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCMessage> &&request);

    std::vector<unsigned char> m_receiveBuffer;
    HANDLE m_handle;
    std::thread m_requestReceivingThread;
};

#endif


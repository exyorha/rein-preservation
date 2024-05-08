#ifndef WEB_VIEW_RPC_SERVER_LINUX_H
#define WEB_VIEW_RPC_SERVER_LINUX_H

#include <cstdint>
#include <thread>

#include "WebViewRPCServer.h"
#include "WebViewSharedImageBuffer.h"

struct cmsghdr;

class WebViewRPCServerLinux final: public WebViewRPCServer {
public:
    explicit WebViewRPCServerLinux(intptr_t connectedSocket);
    ~WebViewRPCServerLinux();

    std::unique_ptr<WebViewSharedImageBuffer> receiveImageBuffer(intptr_t handle) override;

private:
    class LinuxSharedImageBuffer final : public WebViewSharedImageBuffer {
    public:
        explicit LinuxSharedImageBuffer(int fd);
        ~LinuxSharedImageBuffer() override;

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

    int m_socket;
    WebViewRPCService m_service;
    std::vector<unsigned char> m_receiveBuffer;
    std::vector<unsigned char> m_controlBuffer;
    std::thread m_requestReceivingThread;
    const cmsghdr *m_fds;
};

#endif

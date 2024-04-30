#ifndef WEB_VIEW_RPC_SERVER_LINUX_H
#define WEB_VIEW_RPC_SERVER_LINUX_H

#include <cstdint>
#include <thread>

#include "WebViewRPCServer.h"

class WebViewRPCServerLinux final: public WebViewRPCServer {
public:
    explicit WebViewRPCServerLinux(intptr_t connectedSocket);
    ~WebViewRPCServerLinux();

private:
    void receiveRequests();

    void runMainLoop();

    void executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request);
    bool executeRPCCallAndSendResponse(std::unique_ptr<webview::protocol::RPCRequest> &&request);

    bool ensuredRead(void *dest, size_t size);
    bool ensuredWrite(const void *dest, size_t size);
    bool pollFor(int condition);

    int m_socket;
    WebViewRPCService m_service;
    std::vector<unsigned char> m_receiveBuffer;
    std::thread m_requestReceivingThread;
};

#endif

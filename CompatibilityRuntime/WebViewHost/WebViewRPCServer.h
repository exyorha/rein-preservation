#ifndef WEB_VIEW_RPC_SERVER_H
#define WEB_VIEW_RPC_SERVER_H

#include <cstdint>
#include <thread>

#include "WebViewRPCService.h"

class WebViewRPCServer {
public:
    explicit WebViewRPCServer(intptr_t connectedSocket);
    ~WebViewRPCServer();

    WebViewRPCServer(const WebViewRPCServer &other) = delete;
    WebViewRPCServer &operator =(const WebViewRPCServer &other) = delete;

private:
    void receiveRequests();

    void runMainLoop();

    void executeRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request);
    bool doExecuteRPCCall(std::unique_ptr<webview::protocol::RPCRequest> &&request);

    bool ensuredRead(void *dest, size_t size);
    bool ensuredWrite(const void *dest, size_t size);
    bool pollFor(int condition);

    int m_socket;
    WebViewRPCService m_service;
    std::vector<unsigned char> m_receiveBuffer;
    std::thread m_requestReceivingThread;
};

#endif

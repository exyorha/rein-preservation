#ifndef LLSERVICES_NETWORKING_HTTP_SERVER_H
#define LLSERVICES_NETWORKING_HTTP_SERVER_H

#include "LLServices/Networking/EventTypes.h"

namespace LLServices {

    class EventLoop;
    class HttpRequestHandler;
    class LogFacility;

    class HttpServer {
    public:
        HttpServer(EventLoop *eventLoop, HttpRequestHandler *handler);
        ~HttpServer();

        HttpServer(const HttpServer &other) = delete;
        HttpServer &operator =(const HttpServer &other) = delete;

        void listen(const char *address, unsigned int port);
        void acceptConnections(evutil_socket_t fd);

    private:
        static void requestCallback(struct evhttp_request *request, void *context) noexcept;

        EvHTTPPtr m_http;
        HttpRequestHandler *m_handler;
    };

    extern LogFacility LogHttpServer;
}

#endif

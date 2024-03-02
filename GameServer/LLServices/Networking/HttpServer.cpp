#include "LLServices/Networking/HttpServer.h"
#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/HttpRequestHandler.h"

#include "LLServices/Networking/EventLoop.h"

#include "LLServices/Logging/LogFacility.h"

namespace LLServices {

    LogFacility LogHttpServer("HttpServer");

    HttpServer::HttpServer(EventLoop *eventLoop, HttpRequestHandler *handler) : m_handler(handler) {
        auto raw = evhttp_new(eventLoop->base());
        if(!raw)
            throw std::bad_alloc();

        m_http.reset(raw);

        evhttp_set_gencb(m_http.get(), requestCallback, this);
    }

    HttpServer::~HttpServer() = default;

    void HttpServer::listen(const char *address, unsigned int port) {
        auto result = evhttp_bind_socket(m_http.get(), address, port);
        if(result < 0)
            throw std::runtime_error("evhttp_bind_socket has failed");
    }

    void HttpServer::requestCallback(struct evhttp_request *request, void *context) noexcept {
        auto this_ = static_cast<HttpServer *>(context);

        try {

            HttpRequest requestObject(request);

            this_->m_handler->handle(std::move(requestObject));

        } catch(const std::exception &e) {

            evhttp_send_error(request, 500, "An unexpected error has occurred while processing the request.");

            LogHttpServer.error("an exception has occurred while processing a request: %s", e.what());
        } catch(...) {

            evhttp_send_error(request, 500, "An unexpected error has occurred while processing the request.");

            LogHttpServer.error("an exception has occurred while processing a request");
        }

    }
}


#ifndef LLSERVICES_NETWORKING_HTTP_REQUEST_HANDLER_H
#define LLSERVICES_NETWORKING_HTTP_REQUEST_HANDLER_H

namespace LLServices {

    class HttpRequest;

    class HttpRequestHandler {
    protected:
        HttpRequestHandler();
        ~HttpRequestHandler();

    public:
        HttpRequestHandler(const HttpRequestHandler &other) = delete;
        HttpRequestHandler &operator =(const HttpRequestHandler &other) = delete;

        virtual void handle(HttpRequest &&request) = 0;
    };

}

#endif

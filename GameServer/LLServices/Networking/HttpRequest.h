#ifndef NETWORKING_HTTP_REQUEST_H
#define NETWORKING_HTTP_REQUEST_H

#include <LLServices/Networking/EventTypes.h>

struct evhttp_request;

namespace LLServices {

    class URI;
    class KeyValuePairs;
    class Buffer;
    class WebSocketConnection;
    class WebSocketConnectionListener;

    class HttpRequest {
    public:
        HttpRequest() noexcept;
        HttpRequest(evhttp_request *handle);
        ~HttpRequest();

        HttpRequest(const HttpRequest &other) = delete;
        HttpRequest &operator =(const HttpRequest &other) = delete;

        HttpRequest(HttpRequest &&other) noexcept;
        HttpRequest &operator =(HttpRequest &&other) noexcept;

        inline operator bool() const {
            return m_handle;
        }

        void keepAfterCompletion();

        inline evhttp_cmd_type command() const {
            return evhttp_request_get_command(m_handle);
        }

        inline const char *host() const {
            return evhttp_request_get_host(m_handle);
        }

        URI uri() const;

        inline const char *uriString() const {
            return evhttp_request_get_uri(m_handle);
        }

        inline evbuffer *inputBuffer() const {
            return evhttp_request_get_input_buffer(m_handle);
        }

        KeyValuePairs inputHeaders() const;

        inline evbuffer *outputBuffer() const {
            return evhttp_request_get_output_buffer(m_handle);
        }

        KeyValuePairs outputHeaders() const;

        void sendError(int error, const char *reason);
        void sendReply(int error, const char *reason, Buffer &response);

        void sendReplyStartChunked(int error, const char *reason);
        void sendReplyChunk(Buffer &chunk);
        void sendReplyEnd();

        WebSocketConnection startWebSocket(WebSocketConnectionListener *listener, int options = 0);

    private:

        evhttp_request *m_handle;
    };

}

#endif


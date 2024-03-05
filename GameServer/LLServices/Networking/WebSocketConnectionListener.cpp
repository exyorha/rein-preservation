#include <LLServices/Networking/WebSocketConnectionListener.h>

namespace LLServices {

    WebSocketConnectionListener::WebSocketConnectionListener() = default;

    WebSocketConnectionListener::~WebSocketConnectionListener() = default;

    void WebSocketConnectionListener::messageCallback(
        struct evws_connection *connection,
        int type,
        const unsigned char *data,
        size_t length,
        void *context) {

        (void)connection;

        static_cast<WebSocketConnectionListener *>(context)->onMessage(type, data, length);
    }

    void WebSocketConnectionListener::closeCallback(struct evws_connection *connection, void *context) {
        (void)connection;

        static_cast<WebSocketConnectionListener *>(context)->onClose();
    }

}


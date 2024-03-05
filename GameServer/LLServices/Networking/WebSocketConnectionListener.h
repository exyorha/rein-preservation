#ifndef LLSERVICES_NETWORKING_WEB_SOCKET_CONNECTION_LISTENER_H
#define LLSERVICES_NETWORKING_WEB_SOCKET_CONNECTION_LISTENER_H

#include <cstring>

#include <event2/http.h>
#include <event2/ws.h>

namespace LLServices {

    class WebSocketConnectionListener {
    protected:
        WebSocketConnectionListener();
        ~WebSocketConnectionListener();

        WebSocketConnectionListener(const WebSocketConnectionListener &other) = delete;
        WebSocketConnectionListener &operator =(const WebSocketConnectionListener &other) = delete;

    public:
        static void messageCallback(struct evws_connection *connection, int type, const unsigned char *data, size_t length, void *context);
        static void closeCallback(struct evws_connection *connection, void *context);

        inline void *contextPointerForCallbacks() {
            return this;
        }

    protected:
        virtual void onMessage(int type, const unsigned char *data, size_t length) = 0;
        virtual void onClose() = 0;
    };
}

#endif

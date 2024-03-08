#ifndef LLSERVICES_NETWORKING_WEB_SOCKET_CONNECTION_H
#define LLSERVICES_NETWORKING_WEB_SOCKET_CONNECTION_H

#include <LLServices/Networking/EventTypes.h>

namespace LLServices {

    class WebSocketConnection {
    public:
        WebSocketConnection() noexcept;
        explicit WebSocketConnection(EvWSConnectionPtr &&connection);
        ~WebSocketConnection();

        WebSocketConnection(const WebSocketConnection &other) = delete;
        WebSocketConnection &operator =(const WebSocketConnection &other) = delete;

        WebSocketConnection(WebSocketConnection &&other) noexcept;
        WebSocketConnection &operator =(WebSocketConnection &&other) noexcept;

        inline explicit operator bool() const {
            return static_cast<bool>(m_connection);
        }

        void sendText(const char *text);
        void sendBinary(const char *packetData, size_t packetLen);

    private:
        EvWSConnectionPtr m_connection;
    };

}

#endif

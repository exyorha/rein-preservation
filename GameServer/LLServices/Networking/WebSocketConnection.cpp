#include <LLServices/Networking/WebSocketConnection.h>

namespace LLServices {

    WebSocketConnection::WebSocketConnection() noexcept = default;

    WebSocketConnection::WebSocketConnection(EvWSConnectionPtr &&connection) : m_connection(std::move(connection)) {

    }

    WebSocketConnection::~WebSocketConnection() = default;

    WebSocketConnection::WebSocketConnection(WebSocketConnection &&other) noexcept = default;

    WebSocketConnection &WebSocketConnection::operator =(WebSocketConnection &&other) noexcept = default;

    void WebSocketConnection::sendText(const char *text) {
        evws_send_text(m_connection.get(), text);
    }

    void WebSocketConnection::sendBinary(const char *packetData, size_t packetLen) {
        evws_send_binary(m_connection.get(), packetData, packetLen);
    }
}

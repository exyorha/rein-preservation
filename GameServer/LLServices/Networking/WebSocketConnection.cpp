#include <LLServices/Networking/WebSocketConnection.h>

namespace LLServices {

    WebSocketConnection::WebSocketConnection() noexcept = default;

    WebSocketConnection::WebSocketConnection(EvWSConnectionPtr &&connection) : m_connection(std::move(connection)) {

    }

    WebSocketConnection::~WebSocketConnection() = default;

    WebSocketConnection::WebSocketConnection(WebSocketConnection &&other) noexcept = default;

    WebSocketConnection &WebSocketConnection::operator =(WebSocketConnection &&other) noexcept = default;

}

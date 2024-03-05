#include "ServerCLIConnection.h"

ServerCLIConnection::ServerCLIConnection() : m_log(LLServices::LogFacility::instanceName("ServerCLIConnection", this)) {
    m_log.debug("created");
}

ServerCLIConnection::~ServerCLIConnection() {
    m_log.debug("destroyed");
}

void ServerCLIConnection::attachConnection(LLServices::WebSocketConnection &&connection) {
    m_connection = std::move(connection);

    if(!m_connection)
        return;

    m_log.debug("a connection is now attached");
}

void ServerCLIConnection::onMessage(int type, const unsigned char *data, size_t length) {
    m_log.debug("message: type = %d, data = %p, length = %zu\n", type, data, length);
}

void ServerCLIConnection::onClose() {
    m_log.debug("close");
    delete this;
}

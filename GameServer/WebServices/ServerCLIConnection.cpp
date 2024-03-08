#include "WebServices/ServerCLIConnection.h"
#include "WebServices/ServerCLIService.h"

ServerCLIConnection::ServerCLIConnection(ServerCLIService *service) :
    m_serviceRegistration(service, this),
    m_log(LLServices::LogFacility::instanceName("ServerCLIConnection", this)) {
}

ServerCLIConnection::~ServerCLIConnection() = default;

void ServerCLIConnection::attachConnection(LLServices::WebSocketConnection &&connection) {
    m_connection = std::move(connection);

    if(!m_connection)
        return;

    std::array<LLServices::RingBuffer::ConstSegment, 2> segments;
    m_serviceRegistration.service()->logBuffer().getSegmentsForRead(segments);

    if(segments[0].second > 0)
        m_connection.sendBinary(reinterpret_cast<const char *>(segments[0].first), segments[0].second);

    if(segments[1].second > 0)
        m_connection.sendBinary(reinterpret_cast<const char *>(segments[1].first), segments[1].second);
}

void ServerCLIConnection::onMessage(int type, const unsigned char *data, size_t length) {
    std::string_view command(reinterpret_cast<const char *>(data), length);

    m_serviceRegistration.service()->cli().execute(command);
}

void ServerCLIConnection::onClose() {
    delete this;
}

void ServerCLIConnection::emitLogMessage(const std::string_view &inputMessage) {
    if(m_connection)
        m_connection.sendBinary(inputMessage.data(), inputMessage.size());
}

ServerCLIConnection::Registration::Registration(ServerCLIService *service, ServerCLIConnection *connection) : m_service(service),
    m_connection(connection) {

    m_service->addConnection(m_connection);
}

ServerCLIConnection::Registration::~Registration()  {

    m_service->removeConnection(m_connection);
}

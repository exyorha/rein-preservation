#ifndef WEB_SERVICES_SERVER_CLI_CONNECTION_H
#define WEB_SERVICES_SERVER_CLI_CONNECTION_H

#include <LLServices/Networking/WebSocketConnectionListener.h>
#include <LLServices/Networking/WebSocketConnection.h>
#include <LLServices/Logging/LogFacility.h>

class ServerCLIConnection final : public LLServices::WebSocketConnectionListener {
public:
    ServerCLIConnection();
    ~ServerCLIConnection();

    ServerCLIConnection(const ServerCLIConnection &other) = delete;
    ServerCLIConnection &operator =(ServerCLIConnection &other) = delete;

    void attachConnection(LLServices::WebSocketConnection &&connection);

protected:
    void onMessage(int type, const unsigned char *data, size_t length) override;
    void onClose() override;

private:
    LLServices::LogFacility m_log;
    LLServices::WebSocketConnection m_connection;
};

#endif

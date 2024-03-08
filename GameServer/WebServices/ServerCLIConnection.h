#ifndef WEB_SERVICES_SERVER_CLI_CONNECTION_H
#define WEB_SERVICES_SERVER_CLI_CONNECTION_H

#include <LLServices/Networking/WebSocketConnectionListener.h>
#include <LLServices/Networking/WebSocketConnection.h>
#include <LLServices/Logging/LogFacility.h>

class ServerCLIService;

class ServerCLIConnection final : public LLServices::WebSocketConnectionListener {
public:
    explicit ServerCLIConnection(ServerCLIService *service);
    ~ServerCLIConnection();

    ServerCLIConnection(const ServerCLIConnection &other) = delete;
    ServerCLIConnection &operator =(ServerCLIConnection &other) = delete;

    void attachConnection(LLServices::WebSocketConnection &&connection);

    void emitLogMessage(const std::string_view &inputMessage);

protected:
    void onMessage(int type, const unsigned char *data, size_t length) override;
    void onClose() override;

private:
    class Registration {
    public:
        explicit Registration(ServerCLIService *service, ServerCLIConnection *connection);
        ~Registration();

        Registration(const Registration &other) = delete;
        Registration &operator =(const Registration &other) = delete;

        inline ServerCLIService *service() const {
            return m_service;
        }

    private:
        ServerCLIService *m_service;
        ServerCLIConnection *m_connection;
    };

    Registration m_serviceRegistration;
    LLServices::LogFacility m_log;
    LLServices::WebSocketConnection m_connection;
};

#endif

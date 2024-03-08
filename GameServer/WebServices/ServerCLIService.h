#ifndef WEB_SERVICES_SERVER_CLI_SERVICE_H
#define WEB_SERVICES_SERVER_CLI_SERVICE_H

#include <WebServices/WebRoutable.h>

#include <LLServices/Logging/LogSink.h>
#include <LLServices/RingBuffer.h>

#include "ServerCommandLine.h"

#include <vector>

class ServerCLIConnection;

class ServerCLIService final : public WebRoutable, public LLServices::LogSink {
public:
    explicit ServerCLIService(LLServices::LogSink *nextLogSink = nullptr);
    ~ServerCLIService();

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

    void emitMessage(const std::string_view &message) noexcept override;

    inline LLServices::RingBuffer &logBuffer() {
        return m_logBuffer;
    }

    void addConnection(ServerCLIConnection *connection);
    void removeConnection(ServerCLIConnection *connection);

    inline ServerCommandLine &cli() {
        return m_cli;
    }

private:
    LLServices::LogSink *m_nextLogSink;
    std::vector<unsigned char> m_logBufferStorage;
    LLServices::RingBuffer m_logBuffer;
    std::vector<ServerCLIConnection *> m_connections;
    ServerCommandLine m_cli;
};

#endif

#ifndef WEB_SERVICES_SERVER_CLI_SERVICE_H
#define WEB_SERVICES_SERVER_CLI_SERVICE_H

#include <WebServices/WebRoutable.h>

#include <LLServices/Logging/LogSink.h>
#include <LLServices/RingBuffer.h>
#include <LLServices/Networking/BeforeWaitListener.h>
#include <LLServices/Networking/BeforeWaitNotifier.h>

#include "ServerCommandLine.h"

#include <vector>
#include <optional>

class Database;
class ServerCLIConnection;

class ServerCLIService final : public WebRoutable, public LLServices::LogSink, private LLServices::BeforeWaitListener {
public:
    explicit ServerCLIService(LLServices::EventLoop *eventLoop, LLServices::LogSink *nextLogSink = nullptr);
    ~ServerCLIService();

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

    void emitMessage(const std::string_view &message) noexcept override;

    inline LLServices::RingBuffer &logBuffer() {
        return m_logBuffer;
    }

    void addConnection(ServerCLIConnection *connection);
    void removeConnection(ServerCLIConnection *connection);

    inline ServerCommandLine &cli() {
        return m_cli.value();
    }

    void initCLI(Database &db, const GameServerConfiguration &config);

private:
    void beforeWait(LLServices::BeforeWaitNotifier *notifier) override;

private:
    LLServices::LogSink *m_nextLogSink;
    std::vector<unsigned char> m_logBufferStorage;
    LLServices::RingBuffer m_logBuffer;
    std::vector<ServerCLIConnection *> m_connections;
    std::optional<ServerCommandLine> m_cli;
    LLServices::BeforeWaitNotifier m_logUpdateNotifier;
    std::optional<LLServices::RingBuffer::Pointer> m_logBufferWritePointerAtStartOfLogUpdate;
};

#endif

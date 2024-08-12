#include "ServerCLIService.h"
#include "ServerCLIConnection.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/WebSocketConnection.h"
#include "LLServices/Networking/KeyValuePairs.h"

#include <algorithm>

ServerCLIService::ServerCLIService(LLServices::EventLoop *eventLoop, LLServices::LogSink *nextLogSink) :
    m_nextLogSink(nextLogSink),
    m_logBufferStorage(512 * 1024),
    m_logBuffer(m_logBufferStorage.data(), m_logBufferStorage.size()),
    m_logUpdateNotifier(eventLoop, this) {

}

ServerCLIService::~ServerCLIService() = default;

void ServerCLIService::handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) {
    if(routedPath != "/") {
        request.sendError(404, "Not Found");
        return;
    }

    if(request.command() != EVHTTP_REQ_GET) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    /*
     * This is a hack for firefox that does 'Connection: keep-alive, upgrade', while libevent only understands strictly 'Connection: upgrade'
     */
    request.inputHeaders().remove("Connection");
    request.inputHeaders().add("Connection", "upgrade");

    auto cliConnection = std::make_unique<ServerCLIConnection>(this);

    auto connection = request.startWebSocket(cliConnection.get());
    if(!connection) {
        /*
         * A failure response was sent.
         */
        return;
    }

    cliConnection->attachConnection(std::move(connection));

    /*
     * ServerCLIConnection will self-manage its lifetime now.
     */
    (void)cliConnection.release();
}

void ServerCLIService::emitMessage(const std::string_view &inputMessage) noexcept {
    std::string_view message = inputMessage;

    if(m_nextLogSink)
        m_nextLogSink->emitMessage(message);

    if(!m_logBufferWritePointerAtStartOfLogUpdate.has_value()) {
        m_logBufferWritePointerAtStartOfLogUpdate.emplace(m_logBuffer.writePointer());
    }


    auto maximumAvailableSize = m_logBuffer.size() - 1;
    if(message.size() > maximumAvailableSize)
        message = message.substr(message.size() - maximumAvailableSize, maximumAvailableSize);

    auto available = m_logBuffer.bytesAvailableForWrite();
    if(available < inputMessage.size()) {
        m_logBuffer.readData(nullptr, inputMessage.size() - available, LLServices::RingBuffer::TransferType::Normal);
    }

    if(m_logBufferWritePointerAtStartOfLogUpdate.has_value()) {
        auto available = m_logBuffer.bytesAvailableForWrite(*m_logBufferWritePointerAtStartOfLogUpdate);

        if(available < inputMessage.size()) {
            m_logBufferWritePointerAtStartOfLogUpdate.emplace(
                (*m_logBufferWritePointerAtStartOfLogUpdate + inputMessage.size() - available) % m_logBuffer.size()
            );
        }
    }

    m_logBuffer.writeData(reinterpret_cast<const unsigned char *>(inputMessage.data()), inputMessage.size(),
                          LLServices::RingBuffer::TransferType::Normal);

}

void ServerCLIService::addConnection(ServerCLIConnection *connection) {
    m_connections.emplace_back(connection);
}

void ServerCLIService::removeConnection(ServerCLIConnection *connection) {
    m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), connection), m_connections.end());
}

void ServerCLIService::initCLI(Database &db, const GameServerConfiguration &config) {
    m_cli.emplace(db, config);
}

void ServerCLIService::beforeWait(LLServices::BeforeWaitNotifier *notifier) {
    (void)notifier;

    if(m_logBufferWritePointerAtStartOfLogUpdate.has_value()) {
        auto startPointer = *m_logBufferWritePointerAtStartOfLogUpdate;
        m_logBufferWritePointerAtStartOfLogUpdate.reset();

        std::array<LLServices::RingBuffer::ConstSegment, 2> segments;
        m_logBuffer.getSegmentsForRead(segments, startPointer);

        for(const auto &connection :m_connections) {
            for(const auto &segment: segments) {
                if(segment.second != 0) {
                    connection->emitLogMessage(std::string_view(reinterpret_cast<const char *>(segment.first), segment.second));
                }
            }
        }
    }

}

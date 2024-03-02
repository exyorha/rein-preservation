#ifndef LLSERVICES_LOGGING_CONSOLE_LOG_SINK_H
#define LLSERVICES_LOGGING_CONSOLE_LOG_SINK_H

#include <LLServices/Logging/LogSink.h>

#include <climits>

namespace LLServices {
    class ConsoleLogSink final : public LogSink {
    public:
        ConsoleLogSink();
        ~ConsoleLogSink();

        void emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept override;
    };
}

#endif

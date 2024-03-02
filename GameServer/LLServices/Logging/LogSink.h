#ifndef LLMANAGER_LOGGING_LOG_SINK_H
#define LLMANAGER_LOGGING_LOG_SINK_H

#include <string_view>

#include <LLServices/Logging/LogLevel.h>

namespace LLServices {
    class LogSink {
    protected:
        LogSink();
        ~LogSink();

    public:
        LogSink(const LogSink &other) = delete;
        LogSink &operator =(const LogSink &other) = delete;

        virtual void emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept = 0;
    };
}

#endif

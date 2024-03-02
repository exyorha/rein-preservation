#ifndef LLSERVICES_LOGGING_LOG_MANAGER_H
#define LLSERVICES_LOGGING_LOG_MANAGER_H

#include <LLServices/ThreadSingleton.h>
#include <LLServices/Logging/LogLevel.h>

#include <string_view>

namespace LLServices {
    class LogSink;

    class LogManager final : public ThreadSingleton<LogManager> {
    public:
        explicit LogManager(LogSink *sink);
        ~LogManager();

        void emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept;

    private:
        LogSink *m_sink;
    };

    using LogManagerScope = ThreadSingletonScope<LogManager>;
}

#endif

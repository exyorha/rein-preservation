#include <LLServices/Logging/ConsoleLogSink.h>

#include <stdio.h>
#include <errno.h>

namespace LLServices {
    ConsoleLogSink::ConsoleLogSink() = default;

    ConsoleLogSink::~ConsoleLogSink() = default;

    void ConsoleLogSink::emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept {
        const char *levelStr;

        switch(level) {
            case LogLevel::Debug:
            default:
                levelStr = "debug";
                break;

            case LogLevel::Info:
                levelStr = "info";
                break;

            case LogLevel::Warning:
                levelStr = "warn";
                break;

            case LogLevel::Error:
                levelStr = "error";
                break;
        }

        fprintf(stderr, "%-5s %s: %.*s\n", levelStr, facility, static_cast<int>(message.size()), message.data());
    }
}

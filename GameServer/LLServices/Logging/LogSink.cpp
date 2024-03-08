#include <LLServices/Logging/LogSink.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace LLServices {
    LogSink::LogSink() = default;

    LogSink::~LogSink() = default;

    void LogSink::emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept {
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

        char *rawString;
#if defined(_WIN32)
        int result = __mingw_asprintf(&rawString, "%-5s %s: %.*s\n", levelStr, facility, static_cast<int>(message.size()), message.data());
#else
        int result = asprintf(&rawString, "%-5s %s: %.*s\n", levelStr, facility, static_cast<int>(message.size()), message.data());
#endif
        if(result < 0)
            return;;

        struct StringDeleter {
            void operator()(char *ptr) const {
                free(ptr);
            }
        };

        emitMessage(rawString);
    }
}

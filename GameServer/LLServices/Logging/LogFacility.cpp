#include <LLServices/Logging/LogFacility.h>
#include <LLServices/Logging/LogManager.h>

#include <cstdarg>
#include <cstdio>

#include <sstream>
#include <memory>

namespace LLServices {
    LogFacility::LogFacility(std::string &&name) : m_name(std::move(name)) {

    }

    LogFacility::~LogFacility() = default;

    int LogFacility::message(LogLevel level, const char *fmt, ...) noexcept {
        va_list args;

        va_start(args, fmt);

        int result = vmessage(level, fmt, args);

        va_end(args);

        return result;
    }

    int LogFacility::vmessage(LogLevel level, const char *fmt, va_list args) noexcept {
        char *rawString;
#if defined(_WIN32)
        int result = __mingw_vasprintf(&rawString, fmt, args);
#else
        int result = vasprintf(&rawString, fmt, args);
#endif
        if(result < 0)
            return result;

        struct StringDeleter {
            void operator()(char *ptr) const {
                free(ptr);
            }
        };

        std::unique_ptr<char, StringDeleter> string(rawString);

        emitMessage(level, std::string_view(string.get(), result));

        return result;
    }

    int LogFacility::debug(const char *fmt, ...) noexcept {
        va_list args;

        va_start(args, fmt);

        int result = vdebug(fmt, args);

        va_end(args);

        return result;
    }

    int LogFacility::vdebug(const char *fmt, va_list args) noexcept {
        return vmessage(LogLevel::Debug, fmt, args);
    }

    int LogFacility::info(const char *fmt, ...) noexcept {
        va_list args;

        va_start(args, fmt);

        auto result = vinfo(fmt, args);

        va_end(args);

        return result;
    }

    int LogFacility::vinfo(const char *fmt, va_list args) noexcept {
        return vmessage(LogLevel::Info, fmt, args);
    }

    int LogFacility::warning(const char *fmt, ...) noexcept {
        va_list args;

        va_start(args, fmt);

        auto result = vwarning(fmt, args);

        return result;
    }

    int LogFacility::vwarning(const char *fmt, va_list args) noexcept {
        return vmessage(LogLevel::Warning, fmt, args);
    }

    int LogFacility::error(const char *fmt, ...) noexcept {
        va_list args;

        va_start(args, fmt);

        int result = verror(fmt, args);

        va_end(args);

        return result;
    }

    int LogFacility::verror(const char *fmt, va_list args) noexcept {
        return vmessage(LogLevel::Error, fmt, args);
    }

    std::string LogFacility::instanceName(const char *className, void *instance) {
        std::stringstream name;
        name << className << "(0x";
        name.width(2 * sizeof(instance));
        name.fill('0');
        name << std::hex << reinterpret_cast<uintptr_t>(instance);
        name << ")";

        return name.str();
    }

    void LogFacility::emitMessage(LogLevel level, const std::string_view &message) noexcept {
        size_t startIndex = 0;
        while(startIndex < message.size()) {
            auto endIndex = message.find('\n', startIndex);
            std::string_view thisLine;

            if(endIndex == std::string::npos) {
                thisLine = message.substr(startIndex);
                startIndex = endIndex;
            } else {
                thisLine = message.substr(startIndex, endIndex - startIndex);
                startIndex = endIndex + 1;
            }

            LogManager::get()->emitMessage(level, m_name.c_str(), thisLine);
        }
    }
}

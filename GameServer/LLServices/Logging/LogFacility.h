#ifndef LLSERVICES_LOGGING_LOG_FACILITY_H
#define LLSERVICES_LOGGING_LOG_FACILITY_H

#include <string>
#include <string_view>
#include <cstdarg>

#include <LLServices/Logging/LogLevel.h>

namespace LLServices {
    class LogFacility {
    public:
        explicit LogFacility(std::string &&name);
        ~LogFacility();

        LogFacility(const LogFacility &other) = delete;
        LogFacility &operator =(const LogFacility &other) = delete;

        static std::string instanceName(const char *className, void *instance);

        int message(LogLevel level, const char *fmt, ...) noexcept __attribute__((format(printf, 3, 4)));
        int vmessage(LogLevel level, const char *fmt, va_list args) noexcept __attribute__((format(printf, 3, 0)));

        int debug(const char *fmt, ...) noexcept  __attribute__((format(printf, 2, 3)));
        int vdebug(const char *fmt, va_list args) noexcept __attribute__((format(printf, 2, 0)));

        int info(const char *fmt, ...) noexcept __attribute__((format(printf, 2, 3)));
        int vinfo(const char *fmt, va_list args) noexcept __attribute__((format(printf, 2, 0)));

        int warning(const char *fmt, ...) noexcept __attribute__((format(printf, 2, 3)));
        int vwarning(const char *fmt, va_list args) noexcept __attribute__((format(printf, 2, 0)));

        int error(const char *fmt, ...) noexcept __attribute__((format(printf, 2, 3)));
        int verror(const char *fmt, va_list args) noexcept __attribute__((format(printf, 2, 0)));

    private:
        void emitMessage(LogLevel level, const std::string_view &message) noexcept;

        std::string m_name;
    };
}

#endif

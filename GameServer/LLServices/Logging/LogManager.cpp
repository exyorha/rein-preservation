#include <LLServices/Logging/LogManager.h>
#include <LLServices/Logging/LogSink.h>

namespace LLServices {
    LogManager::LogManager(LogSink *sink) : m_sink(sink) {

    }

    LogManager::~LogManager() = default;

    void LogManager::emitMessage(LogLevel level, const char *facility, const std::string_view &message) noexcept {
        m_sink->emitMessage(level, facility, message);
    }

    template<typename Type>
    thread_local std::shared_ptr<Type> ThreadSingleton<Type>::m_instance;

    template class ThreadSingleton<LogManager>;

}

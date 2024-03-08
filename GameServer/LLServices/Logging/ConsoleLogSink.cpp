#include <LLServices/Logging/ConsoleLogSink.h>

#include <stdio.h>
#include <errno.h>

namespace LLServices {
    ConsoleLogSink::ConsoleLogSink() = default;

    ConsoleLogSink::~ConsoleLogSink() = default;

    void ConsoleLogSink::emitMessage(const std::string_view &message) noexcept {
        fwrite(message.data(), 1, message.size(), stderr);
    }
}

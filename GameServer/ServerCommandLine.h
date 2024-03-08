#ifndef SERVER_COMMAND_LINE_H
#define SERVER_COMMAND_LINE_H

#include <string_view>

namespace LLServices {
    class LogFacility;
}

class ServerCommandLine {
public:
    ServerCommandLine();
    ~ServerCommandLine();

    ServerCommandLine(const ServerCommandLine &other) = delete;
    ServerCommandLine &operator =(const ServerCommandLine &other) = delete;

    void execute(const std::string_view &command);
};

extern LLServices::LogFacility LogCLI;

#endif

#include "ServerCommandLine.h"

#include <LLServices/Logging/LogFacility.h>

LLServices::LogFacility LogCLI("CLI");

ServerCommandLine::ServerCommandLine() = default;

ServerCommandLine::~ServerCommandLine() = default;

void ServerCommandLine::execute(const std::string_view &command) {
    try {

        LogCLI.info("%.*s", static_cast<int>(command.size()), command.data());

    } catch(const std::exception &e) {
        LogCLI.error("an exception has occurred while executing the console command: %s", e.what());
    } catch(...) {
        LogCLI.error("an exception has occurred while executing the console command");
    }
}

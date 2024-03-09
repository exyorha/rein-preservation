#include "ServerCommandLine.h"
#include "DataModel/DatabaseContext.h"
#include "WordListParser.h"

#include "DataModel/Database.h"

#include <LLServices/Logging/LogFacility.h>

#include <ctime>
#include <filesystem>
#include <string_view>

LLServices::LogFacility LogCLI("CLI");

const ServerCommandLine::Command ServerCommandLine::m_commands[]{
    { .cmd = "help", .help = "output the list of supported commands", .handler = &ServerCommandLine::commandHelp },
    { .cmd = "backup", .help = "back up the database (save file). If a name is not specified, it'll be picked automatically",
        .handler = &ServerCommandLine::commandBackup
    }
};

void ServerCommandLine::commandHelp(WordListParser &parser) {
    size_t maxLength = 0;
    for(const auto &command: m_commands) {
        maxLength = std::max<size_t>(maxLength, command.cmd.size());
    }

    maxLength += 3;

    LogCLI.info("Commands supported by this server:");
    for(const auto &command: m_commands) {
        LogCLI.info("%*.*s%.*s",
                    -static_cast<int>(maxLength),
                    static_cast<int>(command.cmd.size()),
                    command.cmd.data(),
                    static_cast<int>(command.help.size()),
                    command.help.data());
    }
}

void ServerCommandLine::commandBackup(WordListParser &parser) {
    std::string backupName;

    if(parser.isAtEnd()) {
        time_t currentTime;
        time(&currentTime);

        struct tm currentTimeParts;
#ifdef _WIN32
        gmtime_s(&currentTimeParts, &currentTime);
#else
        gmtime_r(&currentTime, &currentTimeParts);
#endif

        char time[64];
        strftime(time, sizeof(time), "%Y%m%d_%H%M%S", &currentTimeParts);

        backupName.assign(time);
    } else {
        backupName = parser.getRestOfTheString();
    }




    std::filesystem::path backupLocation = m_db.databaseDirectory() / "backups";

    std::filesystem::create_directories(backupLocation);

    backupLocation /= std::u8string_view(reinterpret_cast<const char8_t *>(backupName.data()), backupName.size());
    backupLocation += ".dbjson";

    DatabaseContext ctx(m_db);
    ctx.writeJSONBackup(backupLocation);

    LogCLI.info("the backup file '%s' has been created and saved at:", backupName.c_str());

    auto finalPath = backupLocation.generic_u8string();
    LogCLI.info("%s", reinterpret_cast<const char *>(finalPath.c_str()));
}

ServerCommandLine::ServerCommandLine(Database &db) : m_db(db) {

}

ServerCommandLine::~ServerCommandLine() = default;

void ServerCommandLine::execute(const std::string_view &command) {
    try {

        LogCLI.info("%.*s", static_cast<int>(command.size()), command.data());

        WordListParser parser(command);
        parser.skipSpaces();

        if(parser.isAtEnd())
            return;

        auto cmdToken = parser.getStringWord();

        for(const auto &command: m_commands) {
            if(command.cmd == cmdToken) {
                (this->*command.handler)(parser);

                return;
            }
        }

        LogCLI.error("unsupported command: '%s'. Try 'help'.", cmdToken.c_str());

    } catch(const std::exception &e) {
        LogCLI.error("an exception has occurred while executing the console command: %s", e.what());
    } catch(...) {
        LogCLI.error("an exception has occurred while executing the console command");
    }
}

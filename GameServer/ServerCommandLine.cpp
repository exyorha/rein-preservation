#include "ServerCommandLine.h"
#include "DataModel/DatabaseContext.h"
#include "DataModel/Sqlite/Transaction.h"
#include "DataModel/Sqlite/Statement.h"

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
    },
    { .cmd = "backups", .help = "output the list of available backups", .handler = &ServerCommandLine::commandBackups },
    { .cmd = "restore", .help = "restore a previous version of the database (save file)", .handler = &ServerCommandLine::commandRestore }
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

std::filesystem::path ServerCommandLine::backupLocation() const {

    std::filesystem::path backupLocation = m_db.databaseDirectory() / "backups";

    std::filesystem::create_directories(backupLocation);

    return backupLocation;
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

    auto backupLocation = this->backupLocation();
    backupLocation /= std::u8string_view(reinterpret_cast<const char8_t *>(backupName.data()), backupName.size());
    backupLocation += ".dbjson";

    DatabaseContext ctx(m_db);
    ctx.writeJSONBackup(backupLocation);

    LogCLI.info("The backup file '%s' has been created and saved at:", backupName.c_str());

    auto finalPath = backupLocation.generic_u8string();
    LogCLI.info("%s", reinterpret_cast<const char *>(finalPath.c_str()));
}

void ServerCommandLine::commandBackups(WordListParser &parser) {

    bool hasAnyFiles = false;

    auto backupLocation = this->backupLocation();

    auto pathU8 = backupLocation.u8string();

    for(const auto &entry: std::filesystem::directory_iterator(backupLocation)) {
        const auto &path = entry.path();

        if(!entry.is_regular_file() || !path.has_extension() || path.extension() != ".dbjson")
            continue;

        if(!hasAnyFiles) {
            hasAnyFiles = true;
            LogCLI.info("Available backup files that can be restored with the 'restore' command:");
        }

        auto name = path.stem().u8string();
        LogCLI.info("%s", reinterpret_cast<const char *>(name.c_str()));
    }

    if(hasAnyFiles) {
        LogCLI.info("\nThe backup files are stored in the following directory: %s",
                    reinterpret_cast<const char *>(pathU8.data()));
    } else {
        LogCLI.info("No backup files have been made yet. Backups can be created with the 'backup' command.");

        LogCLI.info("Once any are made, the backup files will be stored in the following directory: %s",
                    reinterpret_cast<const char *>(pathU8.data()));
    }
}

void ServerCommandLine::commandRestore(WordListParser &parser) {

    if(parser.isAtEnd()) {
        LogCLI.error("Please specify the name of the backup file to be restored. Try 'backups' for the list.");
        return;
    }

    auto name = parser.getRestOfTheString();
    std::u8string_view u8name(reinterpret_cast<const char8_t *>(name.data()), name.size());

    std::filesystem::path backupLocation = this->backupLocation();
    backupLocation /= u8name;
    backupLocation += ".dbjson";

    if(!std::filesystem::exists(backupLocation)) {
        LogCLI.error("The specified backup file doesn't exist. Try 'backups' for the list.");
        return;
    }

    {
        Database restoredDB(std::filesystem::path(), m_db.masterDatabase());
        {
            sqlite::Transaction restoreTransaction(&restoredDB.db());
            DatabaseContext restoredContext(restoredDB);

            restoredContext.restoreJSONBackup(backupLocation);
            restoreTransaction.commit();
        }

        restoredDB.db().checkpoint("main", SQLITE_CHECKPOINT_TRUNCATE);
        restoredDB.db().prepare("VACUUM")->exec();

        m_db.restoreFromDB(restoredDB);
    }

    LogCLI.debug("The backup has been successfully restored.");
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

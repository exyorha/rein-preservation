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
    { .cmd = "restore", .help = "restore a previous version of the database (save file)", .handler = &ServerCommandLine::commandRestore },
    { .cmd = "timetravel", .help = "adjusts the server time to be different from the real-world time",
        .handler = &ServerCommandLine::commandTimeTravel },
    { .cmd = "present", .help = "adjusts the server time back to the real-world time", .handler = &ServerCommandLine::commandPresent },
    { .cmd = "gift", .help = "sends a gift (execute just 'gift' for the syntax details)", .handler = &ServerCommandLine::commandGift },
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

void ServerCommandLine::commandTimeTravel(WordListParser &parser) {
    /*
     * Calculate the time offset using SQLite's time functions, since their
     * syntax is friendly enough.
     */

    std::stringstream query;

    query << "SELECT CAST(round((unixepoch(";

    std::vector<std::string> args;
    while(!parser.isAtEnd()) {
        if(!args.empty())
            query << ", ";

        args.emplace_back(parser.getStringWord());

        query << "?";
    }

    if(!args.empty()) {
        query << ", ";
    }

    query << "'subsec') - unixepoch('subsec')) * 1000) AS INT)";

    auto statement = m_db.db().prepare(query.str());
    int index = 1;
    for(const auto &arg: args) {
        statement->bind(index, arg);
        index++;
    }

    /*
     * On error, this will return an empty string.
     */
    std::optional<int64_t> offset;
    if(!statement->step() || statement->columnType(0) != SQLITE_INTEGER)
        throw std::runtime_error("unable to parse the time specification");

    offset.emplace(statement->columnInt64(0));

    statement->reset();

    if(offset.has_value() && *offset == 0)
        offset.reset();

    m_db.setTimeOffset(offset);
}

void ServerCommandLine::commandPresent(WordListParser &parser) {
    m_db.setTimeOffset(std::nullopt);
}

void ServerCommandLine::commandGift(WordListParser &parser) {
    if(parser.isAtEnd()) {
        LogCLI.info(
            "The gift command add items to the user's gift box, and can be used to easily add items to the game's inventory.\n"
            "\n"
            "Usage: gift <ITEM TYPE> <ITEM ID WITHIN THE TYPE> [EXTRA PARAMETER=EXTRA PARAMETER VALUE]...\n"
            "\n"
            "Example: gift costume 21000\n"
            "Example: gift gold count=10000\n"
            "\n"
            "Item types:\n"
            " - 'costume'\n"
#if 0
            "    Accepts parameter 'level' specifying the level of the costume (default: 1)\n"
            "    Accepts parameter 'exp' specifying the experience of the costume (default: determined based on the level)\n"
            "    Accepts parameter 'limit_break_count' specifying the number of ascensions performed on the costume (default: 0)\n"
            "    Accepts parameter 'active_skill_level' specifying the level of the skill of the costume (default: 1)\n"
#endif
            " - 'weapon'\n"
#if 0
            "   Accepts parameter 'level' specifying the level of the weapon (default: 1)\n"
            "   Accepts parameter 'exp' specifying the experience of the weapon (default: determined based on the level)\n"
            "   Accepts parameter 'limit_break_count' specifying the number of ascensions performed on the weapon (default: 0)\n"
            "   Accepts parameters 'skill<SKILL ID>' specifying the level of the corresponding skill (default: not set)\n"
#endif
            " - 'companion'\n"
#if 0
            "   Accepts parameter 'level' specifying the level of the companion (default: 1)\n"
#endif
            " - 'parts' (memoirs)\n"
            " - 'material'\n"
            " - 'consumable_item'\n"
            " - 'gold': shorthand for 'consumable_item %d'; the item ID doesn't need to be specified\n"
            " - 'paid_gem': no item ID should be specified\n"
            " - 'free_gem': no item ID should be specified\n"
            " - 'important_item'\n"
            " - 'thought' (debris)\n"
            " - 'mission_pass_point'\n"
            " - 'premium_item'\n"
            "\n"
#if 0
            "WARNING: Setting the leveling parameters on items to the values outside of the normal bounds may cause game client errors.\n"
            "         Create a backup if you're not sure about the parameters you're setting.\n"
            "         Not setting any leveling parameters is always safe.\n"
#endif
            "The following parameters apply to all item types:\n"
            "  'count': the number of items to be gifted, default: 1\n"
            "  'gift_text': the ID of the gift message text to be used, default: '13' for 'sent by administration'\n"
            "  'expires': the amount of time in which the gift will expire unless claimed, such as 'expires=\"7 days\"'; default: no expiration\n"
            , DatabaseContext(m_db).consumableItemIdForGold()
        );

        return;
    }


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

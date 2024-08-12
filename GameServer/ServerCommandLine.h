#ifndef SERVER_COMMAND_LINE_H
#define SERVER_COMMAND_LINE_H

#include <string_view>

#include <filesystem>

namespace LLServices {
    class LogFacility;
}

class WordListParser;
class Database;
class DatabaseContext;
class UserContext;
class GameServerConfiguration;

class ServerCommandLine {
public:
    ServerCommandLine(Database &db, const GameServerConfiguration &config);
    ~ServerCommandLine();

    ServerCommandLine(const ServerCommandLine &other) = delete;
    ServerCommandLine &operator =(const ServerCommandLine &other) = delete;

    void execute(const std::string_view &command);

private:

    struct Command {
        const std::string_view cmd;
        const std::string_view help;
        void (ServerCommandLine::*handler)(WordListParser &parser);
    };

    void commandHelp(WordListParser &parser);
    void commandBackup(WordListParser &parser);
    void commandBackups(WordListParser &parser);
    void commandRestore(WordListParser &parser);
    void commandTimeTravel(WordListParser &parser);
    void commandPresent(WordListParser &parser);
    void commandGift(WordListParser &parser);
    void commandGiftDatabase(WordListParser &parser, DatabaseContext &db);
    void commandPortalCage(WordListParser &parser);
    void commandPortalCageUser(WordListParser &parser, UserContext &user);
    void commandAddPremiumItem(WordListParser &parser);
    void commandAddPremiumItemUser(WordListParser &parser, UserContext &user);
    void commandAllWeapons(WordListParser &parser);
    void commandAllWeaponsUser(WordListParser &parser, UserContext &user);
    void commandUpdateUnlocks(WordListParser &parser);
    void commandUpdateUnlocksUser(WordListParser &parser, UserContext &user);

    std::filesystem::path backupLocation() const;

    void runCommandInDatabaseContext(WordListParser &parser, void (ServerCommandLine::*)(WordListParser &parser, DatabaseContext &db));
    void runCommandInUserContext(WordListParser &parser, void (ServerCommandLine::*)(WordListParser &parser, UserContext &user));

    static constexpr int64_t FixedUserID = 1;

    Database &m_db;
    const GameServerConfiguration &m_config;

    static const Command m_commands[];
};

extern LLServices::LogFacility LogCLI;

#endif

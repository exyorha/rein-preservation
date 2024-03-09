#ifndef SERVER_COMMAND_LINE_H
#define SERVER_COMMAND_LINE_H

#include <string_view>

#include <filesystem>

namespace LLServices {
    class LogFacility;
}

class WordListParser;
class Database;

class ServerCommandLine {
public:
    explicit ServerCommandLine(Database &db);
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

    std::filesystem::path backupLocation() const;

    Database &m_db;
    static const Command m_commands[];
};

extern LLServices::LogFacility LogCLI;

#endif

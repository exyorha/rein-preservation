#ifndef DATA_MODEL_DATABASE_H
#define DATA_MODEL_DATABASE_H

#include <filesystem>
#include <mutex>
#include <optional>

#include <DataModel/Sqlite/Database.h>

namespace LLServices {
    class LogFacility;
}

class Database {
public:
    Database(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath);
    ~Database();

    Database(const Database &other) = delete;
    Database &operator =(const Database &other) = delete;

    inline sqlite::Database &db() {
        return m_db;
    }

    std::mutex callMutex;

    inline const std::string &masterDatabaseVersion() const {
        return m_masterDatabaseVersion;
    }

    time_t realWorldTime() const;

    inline std::optional<time_t> timeOffset() const {
        return m_timeOffset;
    }

    std::filesystem::path databaseDirectory() const;

private:
    sqlite::Database m_db;

    struct DatabaseMigration {
        int fromVersion;
        int toVersion;
        const char* sql;
    };

    void createDatabaseBackup();

    std::filesystem::path backupDatabasePath() const;

    bool restoreDatabaseBackup();

    static void removeDatabase(const std::filesystem::path &path);

    static int statementProfileCallback(unsigned int type, void *context, void *statement, void *sql);

    void runMigrations();

    static const char *const m_initSQL;
    static const char* const m_setupQueries[];
    static const DatabaseMigration m_migrations[];
    static const unsigned int m_currentSchemaVersion;
    std::string m_masterDatabaseVersion;
    std::optional<time_t> m_timeOffset;
};

extern LLServices::LogFacility LogDatabase;

#endif

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

    int64_t realWorldTime() const;
    int64_t serverTime() const;

    inline std::optional<int64_t> timeOffset() const {
        return m_timeOffset;
    }

    inline void setTimeOffset(const std::optional<int64_t> &timeOffset) {
        setTimeOffset(timeOffset, true);
    }

    std::filesystem::path databaseDirectory() const;

    inline const std::filesystem::path &masterDatabase() const {
        return m_masterDatabase;
    }

    void restoreFromDB(Database &other);

    inline bool isQueryLoggingEnabled() const {
        return m_queryLoggingEnabled;
    }

    inline void setQueryLoggingEnabled(bool queryLoggingEnabled) {
        m_queryLoggingEnabled = queryLoggingEnabled;
    }

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

    void runMigrations(bool transient);

    void loadDynamicConfig();

    void setTimeOffset(const std::optional<int64_t> &timeOffset, bool saveToDatabase);

    static const char *const m_initSQL;
    static const char* const m_setupQueries[];
    static const DatabaseMigration m_migrations[];
    static const unsigned int m_currentSchemaVersion;
    std::string m_masterDatabaseVersion;
    std::optional<int64_t> m_timeOffset;
    std::filesystem::path m_masterDatabase;
    bool m_queryLoggingEnabled;
};

extern LLServices::LogFacility LogDatabase;

#endif

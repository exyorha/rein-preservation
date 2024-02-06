#ifndef DATA_MODEL_DATABASE_H
#define DATA_MODEL_DATABASE_H

#include <filesystem>

#include <DataModel/Sqlite/Database.h>

class Database {
public:
    Database(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath);
    ~Database();

    Database(const Database &other) = delete;
    Database &operator =(const Database &other) = delete;

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

    static const char* const m_setupQueries[];
    static const DatabaseMigration m_migrations[];
    static const unsigned int m_currentSchemaVersion;
};

#endif

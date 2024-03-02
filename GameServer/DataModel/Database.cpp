#include "Database.h"
#include "LLServices/Logging/LogFacility.h"

#include <master_database_ext.h>

#include <DataModel/Sqlite/Error.h>
#include <DataModel/Sqlite/Statement.h>
#include <DataModel/Sqlite/Transaction.h>
#include <DataModel/Sqlite/Backup.h>

#include <DataModel/CurrentNETTimestampFunction.h>

#include <optional>
#include <cstring>

const char* const Database::m_setupQueries[]{
    "PRAGMA journal_mode = WAL",
    "PRAGMA foreign_keys = ON",
    "CREATE TABLE IF NOT EXISTS schema_version (id INTEGER NOT NULL PRIMARY KEY, version INTEGER NOT NULL)",
    "INSERT OR IGNORE INTO schema_version (id, version) VALUES (1, 0)"
};

LLServices::LogFacility LogDatabase("Database");

Database::Database(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
    m_db(individualDatabasePath) {

    LogDatabase.info("Individual database: '%s', master database: '%s'\n", individualDatabasePath.generic_string().c_str(),
                     masterDatabasePath.generic_string().c_str());

    struct ErrorHolder {
        char *error = nullptr;

        ~ErrorHolder() {
            sqlite3_free(error);
        }
    } error;
    auto ret = sqlite3_masterdatabaseext_init(m_db.handle(), &error.error, nullptr);
    if(ret != SQLITE_OK) {
        if(error.error) {
            throw std::runtime_error("Failed to initialize the master database extension: " + std::string(error.error));
        } else {
            throw std::runtime_error("Failed to initialize the master database extension: " + std::to_string(ret));
        }
    }

    {
        auto statement = m_db.prepare("UPDATE masterdatabase_ctrl SET database_filename = ?");
        statement->bind(1, masterDatabasePath.string());
        statement->step();
        statement->reset();
    }

    m_db.createFunction(std::make_unique<CurrentNETTimestampFunction>());

    sqlite3_trace_v2(m_db.handle(), SQLITE_TRACE_STMT, statementProfileCallback, this);

    const char* query = m_initSQL;
    while (*query) {
        while(isspace(*query))
            query++;

        if(!*query)
            break;

        std::string_view view(query);
        static const std::string_view runMigrationsQuery = "RUN MIGRATIONS;";

        if(view.starts_with(runMigrationsQuery)) {
            query += runMigrationsQuery.size();

            runMigrations();
        } else {
            auto statement = m_db.prepare(query, 0, &query);
            while (statement->step());
        }
    }

    auto path = masterDatabasePath.parent_path();
    auto version = path.filename().string();
    path = path.parent_path();
    auto type = path.filename().string();

    std::stringstream fullVersion;
    fullVersion << type << "/" << version;
    m_masterDatabaseVersion = fullVersion.str();
}

void Database::runMigrations() {

    auto restoredFromBackup = restoreDatabaseBackup();

    auto schemaQuery = m_db.prepare("SELECT version FROM schema_version WHERE id = 1");
    int currentVersion = 0;
    if (schemaQuery->step()) {
        currentVersion = schemaQuery->columnInt(0);
    }
    schemaQuery.reset();

    if (currentVersion < m_currentSchemaVersion) {
        LogDatabase.info("Database requires schema update from %d to %d\n", currentVersion, m_currentSchemaVersion);

        if (!restoredFromBackup)
            createDatabaseBackup();

        auto updateSchema = m_db.prepare("UPDATE schema_version SET version = ? WHERE id = 1");

        while (currentVersion != m_currentSchemaVersion) {
            const DatabaseMigration* migration = nullptr;

            for(auto thisMigration = m_migrations; thisMigration->sql; thisMigration++) {
                if (thisMigration->fromVersion == currentVersion && thisMigration->toVersion > thisMigration->fromVersion) {
                    migration = thisMigration;
                    break;
                }
            }

            if (!migration)
                throw std::logic_error("migration not found");

            LogDatabase.info("Migrating database from schema revision %d to %d\n", migration->fromVersion, migration->toVersion);

            sqlite::Transaction transaction(&m_db);

            const char* query = migration->sql;
            while (*query) {
                auto statement = m_db.prepare(query, 0, &query);
                if (statement)
                    while (statement->step());
            }

            updateSchema->reset();
            updateSchema->bind(1, migration->toVersion);
            updateSchema->step();

            transaction.commit();

            currentVersion = migration->toVersion;
        }

        LogDatabase.info("Finished with migrations, removing backup\n");

        removeDatabase(backupDatabasePath());

    }
    else if (currentVersion > m_currentSchemaVersion) {
        LogDatabase.error("Database is newer than expected: expected version %d, current version %d\n",
                          m_currentSchemaVersion, currentVersion);
    }
}

Database::~Database() = default;

int Database::statementProfileCallback(unsigned int type, void *context, void *statementParam, void *sqlParam) {
    if(type == SQLITE_TRACE_STMT) {
        auto statement = static_cast<sqlite3_stmt *>(statementParam);
        auto sql = static_cast<const char *>(sqlParam);

        if(strncmp(sql, "--", 2) == 0) {
            LogDatabase.debug("SQL: %s\n", sql);
        } else {
            struct ManagedString {
                char *ptr = nullptr;

                ~ManagedString() {
                    sqlite3_free(ptr);
                }
            } expandedSql;

            expandedSql.ptr = sqlite3_expanded_sql(statement);
            if(expandedSql.ptr) {

                LogDatabase.debug("SQL: %s\n", expandedSql.ptr);
            } else {

                LogDatabase.debug("SQL: %s\n", sql);
            }
        }

    }

    return 0;
}

void Database::createDatabaseBackup() {
    printf("Creating backup\n");
    std::optional<sqlite::Database> backupDestinationDatabase;
    backupDestinationDatabase.emplace(backupDatabasePath());
    {
        sqlite::Backup backup(*backupDestinationDatabase, "main", m_db, "main");
        backup.step();
    }
    backupDestinationDatabase.reset();
    printf("Backup created\n");
}

bool Database::restoreDatabaseBackup() {
    std::optional<sqlite::Database> backupSourceDatabase;

    try {
        backupSourceDatabase.emplace(backupDatabasePath(), SQLITE_OPEN_READONLY);
    }
    catch (const sqlite::Error& e) {
        if (e.code() == SQLITE_CANTOPEN) {
            return false;
        } else {
            throw;
        }
    }

    printf("Restoring database from backup\n");
    {
        sqlite::Backup backup(m_db, "main", *backupSourceDatabase, "main");
        backup.step();
    }
    backupSourceDatabase.reset();
    printf("Restored successfully\n");
    removeDatabase(backupDatabasePath());

    return true;
}

std::filesystem::path Database::backupDatabasePath() const {
    std::filesystem::path dbPath = m_db.filename("main");

    dbPath.replace_extension(".backup");

    return dbPath;
}

void Database::removeDatabase(const std::filesystem::path& path) {
    std::error_code ec;
    std::filesystem::remove(path, ec);

    auto journalPath = path;
    journalPath += std::filesystem::path("-journal");
    std::filesystem::remove(journalPath);

    auto walPath = path;
    walPath += std::filesystem::path("-wal");
    std::filesystem::remove(walPath);

    auto shmPath = path;
    shmPath += std::filesystem::path("-shm");
    std::filesystem::remove(shmPath);
}

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
#include <system_error>

const char* const Database::m_setupQueries[]{
    "PRAGMA journal_mode = WAL",
    "PRAGMA foreign_keys = ON",
    "CREATE TABLE IF NOT EXISTS schema_version (id INTEGER NOT NULL PRIMARY KEY, version INTEGER NOT NULL)",
    "INSERT OR IGNORE INTO schema_version (id, version) VALUES (1, 0)"
};

LLServices::LogFacility LogDatabase("Database");

Database::Database(const std::filesystem::path &individualDatabasePath, const std::filesystem::path &masterDatabasePath) :
    m_db(individualDatabasePath), m_masterDatabase(masterDatabasePath), m_queryLoggingEnabled(true) {

    bool transient = individualDatabasePath.empty();

    if(!transient)
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

    m_db.createFunction(std::make_unique<CurrentNETTimestampFunction>(*this));

    if(!transient)
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

            runMigrations(transient);
        } else {
            auto statement = m_db.prepare(query, 0, &query);
            while (statement->step());
        }
    }

    m_masterDatabaseVersion = masterDatabasePath.stem().string();

    loadDynamicConfig();

}

void Database::runMigrations(bool transient) {

    bool restoredFromBackup = false;
    if(!transient)
        restoredFromBackup = restoreDatabaseBackup();

    auto schemaQuery = m_db.prepare("SELECT version FROM schema_version WHERE id = 1");
    int currentVersion = 0;
    if (schemaQuery->step()) {
        currentVersion = schemaQuery->columnInt(0);
    }
    schemaQuery.reset();

    if (currentVersion < m_currentSchemaVersion) {
        LogDatabase.info("Database requires schema update from %d to %d\n", currentVersion, m_currentSchemaVersion);

        if (!restoredFromBackup && !transient)
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

        if(!transient)
            removeDatabase(backupDatabasePath());

    }
    else if (currentVersion > m_currentSchemaVersion) {
        LogDatabase.error("Database is newer than expected: expected version %d, current version %d\n",
                          m_currentSchemaVersion, currentVersion);
    }
}

Database::~Database() = default;

int Database::statementProfileCallback(unsigned int type, void *context, void *statementParam, void *sqlParam) {
    if(!static_cast<Database *>(context)->m_queryLoggingEnabled)
        return 0;

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
    LogDatabase.debug("Creating backup\n");
    std::optional<sqlite::Database> backupDestinationDatabase;
    backupDestinationDatabase.emplace(backupDatabasePath());
    {
        sqlite::Backup backup(*backupDestinationDatabase, "main", m_db, "main");
        backup.step();
    }
    backupDestinationDatabase.reset();
    LogDatabase.debug("Backup created\n");
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

    LogDatabase.debug("Restoring database from backup\n");
    {
        sqlite::Backup backup(m_db, "main", *backupSourceDatabase, "main");
        backup.step();
    }
    backupSourceDatabase.reset();
    LogDatabase.debug("Restored successfully\n");
    removeDatabase(backupDatabasePath());

    return true;
}

std::filesystem::path Database::databaseDirectory() const {
    return std::filesystem::path(m_db.filename("main")).parent_path();
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

int64_t Database::realWorldTime() const {
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    return time.count();
}

void Database::restoreFromDB(Database &other) {
    {
        sqlite::Backup backup(m_db, "main", other.db(), "main");
        backup.step();
    }

    m_db.checkpoint("main", SQLITE_CHECKPOINT_TRUNCATE);

    loadDynamicConfig();
}

void Database::loadDynamicConfig() {
    auto query = db().prepare("SELECT time_offset FROM internal_time_offset");

    std::optional<int64_t> timeOffset;
    if(query->step()) {
        timeOffset.emplace(query->columnInt64(0));
    }
    query->reset();

    setTimeOffset(timeOffset, false);
}

void Database::setTimeOffset(const std::optional<int64_t> &timeOffset, bool saveToDatabase) {
    if(saveToDatabase) {
        sqlite::Transaction transaction(&m_db);

        if(timeOffset.has_value()) {
            auto statement = db().prepare(R"SQL(
                INSERT INTO internal_time_offset (
                    rowid,
                    time_offset
                ) VALUES (
                    1,
                    ?
                ) ON CONFLICT (rowid) DO UPDATE SET time_offset = excluded.time_offset
            )SQL");

            statement->bind(1, *timeOffset);
            statement->exec();
        } else {
            auto statement = db().prepare("DELETE FROM internal_time_offset");
            statement->exec();
        }

        transaction.commit();
    }

    m_timeOffset = timeOffset;

    struct MessageAndOffset {
        const char *message;
        int64_t offset;
    };

    auto currentTimeMilliseconds = realWorldTime();

    std::vector<MessageAndOffset> messages;


    if(m_timeOffset.has_value()) {
        LogDatabase.info("The server time is currently configured to be different from the real-world time.");

        messages.emplace_back(MessageAndOffset{ .message = "Current real-world time: %s (UTC)", .offset = 0 });
        messages.emplace_back(MessageAndOffset{ .message = "Current server time:     %s (UTC)", .offset = *timeOffset });
    } else {
        messages.emplace_back(MessageAndOffset{ .message = "Current server time:     %s (UTC)", .offset = 0 });
    }

    for(const auto &message: messages) {

        time_t timeSeconds = (currentTimeMilliseconds + message.offset) / 1000;
        struct tm tmparts;
#ifdef _WIN32
        gmtime_s(&tmparts, &timeSeconds);
#else
        gmtime_r(&timeSeconds, &tmparts);
#endif

        char time[64];
        strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", &tmparts);

        LogDatabase.info(message.message, time);
    }
}

int64_t Database::serverTime() const {
    auto time = realWorldTime();

    if(m_timeOffset.has_value())
        time += *m_timeOffset;

    return time;
}

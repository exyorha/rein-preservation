#include <DataModel/DatabaseContext.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <DataModel/Sqlite/Statement.h>

#include <stdexcept>

DatabaseContext::DatabaseContext(Database &db) : m_db(db) {

}

DatabaseContext::~DatabaseContext() = default;

int32_t DatabaseContext::getIntConfig(const std::string_view &setting) const {
    auto query = db().prepare("SELECT value FROM m_config WHERE config_key = ?");
    query->bind(1, setting);

    if(query->step()) {
        return query->columnInt(0);
    }

    throw std::runtime_error("option is not in config: " + std::string(setting));
}

std::optional<int32_t> DatabaseContext::evaluateNumericalParameterMap(int32_t mapId, int32_t value) {
    auto query = db().prepare(R"SQL(
        SELECT
            parameter_key
        FROM
            m_numerical_parameter_map
        WHERE
            parameter_value <= ? AND
            numerical_parameter_map_id = ?
        ORDER BY
            parameter_key DESC
        LIMIT 1
    )SQL");

    query->bind(1, value);
    query->bind(2, mapId);
    if(query->step()) {
        return query->columnInt(0);
    }

    return {};
}

std::optional<int64_t> DatabaseContext::authenticate(const std::string_view &sessionKey) {
    auto statement = db().prepare("SELECT user_id FROM internal_sessions WHERE session_id = ? AND expire_datetime >= unixepoch('now')");
    statement->bind(1, sessionKey);

    if(statement->step()) {
        return statement->columnInt64(0);
    }

    return std::nullopt;
}


std::vector<std::string> DatabaseContext::getUserDataName() const {
    std::vector<std::string> names;

    auto statement = db().prepare("SELECT name FROM pragma_table_list WHERE schema = 'main' AND type = 'table' AND name LIKE 'i\\_%' ESCAPE '\\'");
    while(statement->step()) {
        names.emplace_back(tableNameToEntityName(statement->columnText(0)));
    }

    return names;
}

int64_t DatabaseContext::registerUser() {

    /*
     * Currently, we only maintain one user with the ID = 1, making it
     * impossible to lose user data. This is limited to this method and
     * the Auth method, though, the rest of the server properly handles
     * user IDs.
     */

    auto statement = db().prepare(R"SQL(
        INSERT OR INTO i_user (
            user_id, register_datetime
        )
        VALUES (
            1, current_net_timestamp()
        )
        ON CONFLICT (user_id) DO NOTHING
    )SQL");

    statement->exec();

    return 1;
}

void DatabaseContext::authenticate(int64_t &outputUserId, std::string &outputSession, time_t &outputExpiration) {

    db().prepare("DELETE FROM internal_sessions WHERE user_id = 1")->exec();

    db().prepare("INSERT OR IGNORE INTO i_user (user_id, register_datetime) VALUES (1, current_net_timestamp())")->exec();

    auto makeSession = db().prepare(R"SQL(
        INSERT INTO internal_sessions (session_id, user_id, expire_datetime) VALUES (hex(randomblob(16)), ?, unixepoch('now', '10 hours'))
        RETURNING session_id, user_id, expire_datetime
    )SQL");

    makeSession->bind(1, 1); // The user ID is always '1' right now

    while(makeSession->step()) {
        outputSession = makeSession->columnText(0);
        outputUserId = makeSession->columnInt(1);
        outputExpiration = makeSession->columnInt(2);
    }

    for(auto userInitQuery: {
        "INSERT OR IGNORE INTO i_user_status (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_gem (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_login (user_id) VALUES (?)", // note that login time is not updated here yet - that's GameStart!
        "INSERT OR IGNORE INTO i_user_profile (user_id) VALUES (?)"

    }) {
        auto query = db().prepare(userInitQuery);
        query->bind(1, outputUserId);
        query->exec();
    }
}



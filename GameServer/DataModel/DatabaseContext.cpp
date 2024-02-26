#include "DataModel/DatabaseEnums.h"
#include <DataModel/DatabaseContext.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <DataModel/Sqlite/Statement.h>

#include <stdexcept>
#include <cmath>

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

std::optional<int32_t> DatabaseContext::getNumericalParameterMapValue(int32_t mapId, int32_t key) {
    auto query = db().prepare(R"SQL(
        SELECT
            parameter_value
        FROM
            m_numerical_parameter_map
        WHERE
            numerical_parameter_map_id = ? AND
            parameter_key = ?
    )SQL");
    query->bind(1, mapId);
    query->bind(2, key);
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

void DatabaseContext::queryCostumeEnhancementCost(int32_t costumeID, int32_t itemCount,
                                                  int32_t &costumeEnhancementCost) {
    auto query = db().prepare(R"SQL(
        SELECT
            m_costume_rarity.enhancement_cost_by_material_numerical_function_id
        FROM
            m_costume,
            m_costume_rarity ON m_costume_rarity.rarity_type = m_costume.rarity_type
        WHERE
            costume_id = ?
    )SQL");

    query->bind(1, costumeID);
    if(!query->step())
        throw std::runtime_error("no such costume");

    auto costFunction = query->columnInt(0);
    query->reset();

    costumeEnhancementCost = evaluateNumericalFunction(costFunction, itemCount);
}

void DatabaseContext::queryWeaponEnhancementCost(int32_t weaponID, int32_t itemCount,
                                                 int32_t &weaponEnhancementCost) {
    auto query = db().prepare(R"SQL(
        SELECT
            m_weapon_rarity.enhancement_cost_by_material_numerical_function_id
        FROM
            m_weapon,
            m_weapon_rarity ON m_weapon_rarity.rarity_type = m_weapon.rarity_type
        WHERE
            weapon_id = ?
    )SQL");

    query->bind(1, weaponID);
    if(!query->step())
        throw std::runtime_error("no such weapon");

    auto costFunction = query->columnInt(0);
    query->reset();

    weaponEnhancementCost = evaluateNumericalFunction(costFunction, itemCount);
}

int32_t DatabaseContext::evaluateNumericalFunction(int32_t functionId, int32_t input) {
    auto queryFunction = db().prepare(R"SQL(
        SELECT
            numerical_function_type,
            numerical_function_parameter_group_id
        FROM
            m_numerical_function
        WHERE
            numerical_function_id = ?
    )SQL");

    queryFunction->bind(1, functionId);
    if(!queryFunction->step())
        throw std::runtime_error("no such numerical function");

    auto type = static_cast<NumericalFunctionType>(queryFunction->columnInt(0));
    auto parameterGroupId = queryFunction->columnInt(1);

    queryFunction->reset();

    std::vector<int32_t> parameterValues;
    if(parameterGroupId != 0) {
        auto queryParameters = db().prepare(R"SQL(
            SELECT
                parameter_value
            FROM
                m_numerical_function_parameter_group
            WHERE
                numerical_function_parameter_group_id = ?
            ORDER BY
                parameter_index
        )SQL");
        queryParameters->bind(1, parameterGroupId);
        while(queryParameters->step()) {
            parameterValues.emplace_back(queryParameters->columnInt(0));
        }

        queryParameters->reset();
    }

    return evaluateNumericalFunction(type, parameterValues, input);
}

int32_t DatabaseContext::evaluateNumericalFunction(NumericalFunctionType type, const std::vector<int32_t> &parameters, int32_t input) {
    switch(type) {
        case NumericalFunctionType::LINEAR:
            if(parameters.size() != 2)
                throw std::logic_error("a linear function doesn't have exactly two parameters");

            return parameters[0] * input + parameters[1];

        default:
            throw std::runtime_error("unsupported numerical function: " + std::to_string(static_cast<int32_t>(type)));
    }
}

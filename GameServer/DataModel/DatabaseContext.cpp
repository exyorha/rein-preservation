#include <DataModel/DatabaseContext.h>
#include <DataModel/DatabaseJSONRepresentation.h>
#include <DataModel/Zlib.h>
#include <DataModel/DatabaseBackupDeserializer.h>
#include <DataModel/Sqlite/Statement.h>

#include <stdexcept>
#include <fstream>

#include <LLServices/JSON/JSONWriter.h>
#include <LLServices/JSON/StreamingJSONParser.h>
#include <LLServices/Logging/LogFacility.h>

#include <service/GiftService.pb.h>

LLServices::LogFacility LogDatabaseContext("DatabaseContext");

DatabaseContext::DatabaseContext(Database &db, const GameServerConfiguration &config) : m_db(db), m_config(config) {

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
    auto statement = db().prepare("SELECT user_id FROM internal_sessions WHERE session_id = ? AND expire_datetime >= current_net_timestamp() / 1000");
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
        INSERT INTO i_user (
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
        INSERT INTO internal_sessions (session_id, user_id, expire_datetime) VALUES (hex(randomblob(16)), ?, unixepoch(current_net_timestamp() / 1000, 'unixepoch', '10 hours'))
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

int32_t DatabaseContext::getWeaponIdForEvolutionOrder(int32_t weaponId, int32_t evolutionOrder) {
    auto queryEvolutionRelation = db().prepare(R"SQL(
        SELECT
            first_evolution.weapon_id
        FROM
            m_weapon,
            m_weapon_evolution_group USING (weapon_id),
            m_weapon_evolution_group AS first_evolution ON
                first_evolution.weapon_evolution_group_id = m_weapon_evolution_group.weapon_evolution_group_id AND
                first_evolution.evolution_order = ?
        WHERE m_weapon.weapon_id = ?
    )SQL");
    queryEvolutionRelation->bind(1, evolutionOrder);
    queryEvolutionRelation->bind(2, weaponId);
    if(queryEvolutionRelation->step()) {
        auto resolvedWeaponId = queryEvolutionRelation->columnInt(0);
        LogDatabaseContext.debug("getWeaponIdForEvolutionOrder(%d, %d): resolved as %d via the evolution relation",
                    weaponId, evolutionOrder, resolvedWeaponId);

        return resolvedWeaponId;
    }

    if(evolutionOrder != 1)
        throw std::runtime_error("the weapon has no such evolution order");

    LogDatabaseContext.debug("getUnevolvedWeaponId(%d): returning self as fallback", weaponId);

    return weaponId;
}

void DatabaseContext::queryWeaponEnhancementCost(int32_t weaponID, int32_t itemCount,
                                                 int32_t &weaponEnhancementCost) {
    auto query = db().prepare(R"SQL(
        SELECT
            COALESCE(
                m_weapon_specific_enhance.enhancement_cost_by_material_numerical_function_id,
                m_weapon_rarity.enhancement_cost_by_material_numerical_function_id
            )
        FROM
            m_weapon,
            m_weapon_rarity USING (rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
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

int32_t DatabaseContext::getMainQuestRouteId(int32_t questId) {

    auto getRoute = db().prepare(R"SQL(
        SELECT
            m_main_quest_chapter.main_quest_route_id
        FROM
            m_main_quest_sequence,
            m_main_quest_sequence_group ON m_main_quest_sequence_group.main_quest_sequence_id = m_main_quest_sequence.main_quest_sequence_id,
            m_main_quest_chapter ON m_main_quest_chapter.main_quest_sequence_group_id = m_main_quest_sequence_group.main_quest_sequence_group_id
        WHERE m_main_quest_sequence.quest_id = ?
    )SQL");

    getRoute->bind(1, questId);
    if(!getRoute->step()) {
        throw std::runtime_error("the quest was not found");
    }

    auto route = getRoute->columnInt64(0);

    if(getRoute->step())
        throw std::runtime_error("this quest belongs to multiple routes");

    getRoute->reset();

    return route;
}

int32_t DatabaseContext::getFirstQuestScene(int32_t questId) {

    auto getFirstScene = db().prepare(R"SQL(
        SELECT
            quest_scene_id
        FROM
            m_quest_scene
        WHERE
            quest_id = ?
        ORDER BY sort_order
        LIMIT 1
    )SQL");

    getFirstScene->bind(1, questId);
    if(!getFirstScene->step())
        throw std::runtime_error("the quest was not found or has no scenes");

    return getFirstScene->columnInt(0);
}

void DatabaseContext::serializeTable(const std::string_view &tableEntityName, LLServices::JSONWriter &json, std::optional<int64_t> limitToUser) {

    json.writeArrayOpen();

    std::stringstream query;
    query << "SELECT * FROM " << entityNameToTableNameChecked(tableEntityName);
    if(limitToUser.has_value()) {
        query << " WHERE user_id = ?";
    }
    auto statement = db().prepare(query.str());
    if(limitToUser.has_value()) {
        statement->bind(1, *limitToUser);
    }

    auto colCount = statement->columnCount();

    std::optional<std::vector<std::string>> columnNames;

    while(statement->step()) {
        json.writeMapOpen();

        if(!columnNames.has_value()) {
            columnNames.emplace();

            columnNames->reserve(colCount);

            for(int colIndex = 0; colIndex < colCount; colIndex++) {
                auto columnName = statement->columnName(colIndex);

                columnNames->emplace_back(columnNameToEntityFieldName(columnName));
            }
        }

        for(int colIndex = 0; colIndex < colCount; colIndex++) {
            const auto &columnName = (*columnNames)[colIndex];

            json.writeString(columnName);

            writeSQLiteColumnValue(json, *statement, colIndex);
        }

        json.writeMapClose();
    }

    json.writeArrayClose();
}

void DatabaseContext::restoreJSONBackup(const std::filesystem::path &input) {
    std::string backupJSON;

    {
        std::string backupData;

        std::ifstream stream;
        stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
        stream.open(input, std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);
        auto length = static_cast<size_t>(stream.tellg());
        backupData.resize(length);

        stream.seekg(0);

        stream.read(backupData.data(), backupData.size());

        backupJSON = inflate(backupData, true);
    }

    DatabaseBackupDeserializer deserializer(db());

    LLServices::StreamingJSONParser parser(deserializer);
    if(!parser.parseChunk(backupJSON) || !parser.finish())
        throw std::runtime_error("failed to parse the backup file");
}

void DatabaseContext::writeJSONBackup(const std::filesystem::path &output) {

    LLServices::JSONWriter writer;
    writer.writeMapOpen();

    for(const auto &table: getUserDataName()) {
        writer.writeString(table);
        serializeTable(table, writer, std::nullopt);
    }

    writer.writeMapClose();

    auto compressed = deflate(writer.output(), true);

    std::ofstream stream;
    stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
    stream.open(output, std::ios::out | std::ios::trunc | std::ios::binary);

    stream.write(compressed.data(), compressed.size());
}

int32_t DatabaseContext::getWeaponMaxLevelForEvolutionOrder(int32_t weaponId, int32_t evolutionOrder) {
    weaponId = getWeaponIdForEvolutionOrder(weaponId, evolutionOrder);

    auto queryMaxLevelFunction = db().prepare(R"SQL(
        SELECT
            COALESCE(
                m_weapon_specific_enhance.max_level_numerical_function_id,
                m_weapon_rarity.max_level_numerical_function_id
            )
        FROM
            m_weapon,
            m_weapon_rarity USING (rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
        WHERE
            weapon_id = ?
    )SQL");
    queryMaxLevelFunction->bind(1, weaponId);
    if(!queryMaxLevelFunction->step())
        throw std::runtime_error("the weapon was not found");

    auto functionId = queryMaxLevelFunction->columnInt(0);
    /*
     * Assuming that 'initial max level' means 'after limit breaks'
     */

    int32_t limitBreaks = getIntConfig("WEAPON_LIMIT_BREAK_AVAILABLE_COUNT");

    return evaluateNumericalFunction(functionId, limitBreaks);
}

bool DatabaseContext::isValidPossession(PossessionType type, int32_t possessionId) {
    switch(type) {
        case PossessionType::COSTUME:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_costume WHERE costume_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::WEAPON:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_weapon WHERE weapon_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::COMPANION:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_companion WHERE companion_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::PARTS:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_parts WHERE parts_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::MATERIAL:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_material WHERE material_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::CONSUMABLE_ITEM:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_consumable_item WHERE consumable_item_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::COSTUME_ENHANCED:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_costume_enhanced WHERE costume_enhanced_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::WEAPON_ENHANCED:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_weapon_enhanced WHERE weapon_enhanced_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::COMPANION_ENHANCED:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_companion_enhanced WHERE companion_enhanced_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::PARTS_ENHANCED:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_parts_enhanced WHERE parts_enhanced_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::PAID_GEM:
        case PossessionType::FREE_GEM:
        case PossessionType::MISSION_PASS_POINT:
            return 1;

        case PossessionType::IMPORTANT_ITEM:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_important_item WHERE important_item_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::THOUGHT:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_thought WHERE thought_item_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        case PossessionType::PREMIUM_ITEM:
        {
            auto query = m_db.db().prepare("SELECT 1 FROM m_premium_item WHERE premium_item_id = ?");
            query->bind(1, possessionId);
            return query->step();
        }

        default:
            throw std::logic_error("unsupported possession type");
    }
}

int64_t DatabaseContext::gift(
    int64_t userId,
    const apb::api::gift::GiftCommon &gift,
    int64_t expiresAt) {

    GiftRewardKindFilterType rewardKindType;

    switch(static_cast<PossessionType>(gift.possession_type())) {
        case PossessionType::UNKNOWN:
            rewardKindType = GiftRewardKindFilterType::UNKNOWN;
            break;

        case PossessionType::COSTUME:
        case PossessionType::COSTUME_ENHANCED:
            rewardKindType = GiftRewardKindFilterType::COSTUME;
            break;

        case PossessionType::WEAPON:
        case PossessionType::WEAPON_ENHANCED:
            rewardKindType = GiftRewardKindFilterType::WEAPON;
            break;

        case PossessionType::COMPANION:
        case PossessionType::COMPANION_ENHANCED:
            rewardKindType = GiftRewardKindFilterType::COMPANION;
            break;

        case PossessionType::PARTS:
        case PossessionType::PARTS_ENHANCED:
            rewardKindType = GiftRewardKindFilterType::PARTS;
            break;

        case PossessionType::MATERIAL:
            rewardKindType = GiftRewardKindFilterType::MATERIAL;
            break;

        case PossessionType::CONSUMABLE_ITEM:
            if(gift.possession_id() == consumableItemIdForGold()) {
                rewardKindType = GiftRewardKindFilterType::GOLD;
            } else {
                rewardKindType = GiftRewardKindFilterType::OTHER;
            }
            break;

        case PossessionType::PAID_GEM:
        case PossessionType::FREE_GEM:
            rewardKindType = GiftRewardKindFilterType::GEM;
            break;

        default:
            rewardKindType = GiftRewardKindFilterType::GOLD;
            break;
    }

    auto query = db().prepare(R"SQL(
        INSERT INTO internal_user_gift (
            user_id,
            grant_datetime,
            expires_datetime,
            gift_data,
            reward_kind_type
        ) VALUES (
            ?,
            current_net_timestamp(),
            ?,
            ?,
            ?
        )
        RETURNING gift_id
    )SQL");

    auto giftDataBlob = gift.SerializeAsString();

    query->bind(1, userId);
    query->bind(2, expiresAt);
    query->bind(3, reinterpret_cast<const unsigned char *>(giftDataBlob.data()), giftDataBlob.size());
    query->bind(4, static_cast<int32_t>(rewardKindType));

    if(query->step()) {
        return query->columnInt64(0);
    }

    return 0;
}


#ifndef DATABASE_CONTEXT_H
#define DATABASE_CONTEXT_H

#include <DataModel/Database.h>
#include <DataModel/DatabaseEnums.h>

#include <optional>
#include <vector>
#include <filesystem>

class Database;

namespace LLServices {
    class JSONWriter;
    class LogFacility;
}

class DatabaseContext {
public:
    explicit DatabaseContext(Database &database);
    virtual ~DatabaseContext();

    DatabaseContext(const DatabaseContext &other) = delete;
    DatabaseContext &operator =(const DatabaseContext &other) = delete;

    inline Database &dataModel() const {
        return m_db;
    }

    inline sqlite::Database &db() const {
        return m_db.db();
    }

    std::vector<std::string> getUserDataName() const;

    int32_t getIntConfig(const std::string_view &setting) const;

    inline int32_t consumableItemIdForGold() const {
        return getIntConfig("CONSUMABLE_ITEM_ID_FOR_GOLD");
    }

    inline int32_t userLevelExpNumericalParameterMapID() const {
        return getIntConfig("USER_LEVEL_EXP_NUMERICAL_PARAMETER_MAP_ID");
    }

    std::optional<int32_t> evaluateNumericalParameterMap(int32_t mapId, int32_t value);
    std::optional<int32_t> getNumericalParameterMapValue(int32_t mapId, int32_t key);
    int32_t evaluateNumericalFunction(int32_t functionId, int32_t input);
    static int32_t evaluateNumericalFunction(NumericalFunctionType type, const std::vector<int32_t> &parameters, int32_t input);

    std::optional<int64_t> authenticate(const std::string_view &sessionKey);

    int64_t registerUser();
    void authenticate(int64_t &outputUserId, std::string &outputSession, time_t &outputExpiration);

    void queryCostumeEnhancementCost(int32_t costumeID, int32_t itemCount, int32_t &costumeEnhancementCost);
    void queryWeaponEnhancementCost(int32_t weaponID, int32_t itemCount, int32_t &weaponEnhancementCost);

    void writeJSONBackup(const std::filesystem::path &output);
    void restoreJSONBackup(const std::filesystem::path &input);

    void serializeTable(const std::string_view &tableEntityName, LLServices::JSONWriter &writer, std::optional<int64_t> limitToUser);

protected:
    int32_t getFirstQuestScene(int32_t questId);
    int32_t getMainQuestRouteId(int32_t questId);

    inline int32_t getUnevolvedWeaponId(int32_t weaponId) {
        return getWeaponIdForEvolutionOrder(weaponId, 1);
    }

    int32_t getWeaponIdForEvolutionOrder(int32_t weaponId, int32_t evolutionOrder);

    int32_t getWeaponMaxLevelForEvolutionOrder(int32_t weaponId, int32_t evolutionOrder);

private:
    Database &m_db;
};

extern LLServices::LogFacility LogDatabaseContext;

#endif

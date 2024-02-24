#ifndef DATABASE_CONTEXT_H
#define DATABASE_CONTEXT_H

#include <DataModel/Database.h>

#include <optional>
#include <vector>

class Database;

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

    std::optional<int64_t> authenticate(const std::string_view &sessionKey);

    int64_t registerUser();
    void authenticate(int64_t &outputUserId, std::string &outputSession, time_t &outputExpiration);

private:
    Database &m_db;
};

#endif

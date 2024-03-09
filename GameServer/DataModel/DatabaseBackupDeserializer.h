#ifndef DATA_MODEL_DATABASE_BACKUP_DESERIALIZER_H
#define DATA_MODEL_DATABASE_BACKUP_DESERIALIZER_H

#include <LLServices/JSON/StreamingJSONCallback.h>

#include <variant>
#include <sstream>
#include <vector>
#include <optional>

namespace sqlite {

    class Database;
    class Statement;

}

class DatabaseBackupDeserializer final : public LLServices::StreamingJSONCallback {
public:
    explicit DatabaseBackupDeserializer(sqlite::Database &db);
    ~DatabaseBackupDeserializer();

    bool null() override;
    bool boolean(bool value) override;
    bool integer(int64_t value) override;
    bool doubleValue(double value) override;
    bool string(const std::string_view &value) override;
    bool startMap() override;
    bool mapKey(const std::string_view &key) override;
    bool endMap() override;
    bool startArray() override;
    bool endArray() override;

private:
    template<typename T, typename... Args>
    bool storeValue(const Args &...args);

    enum class State {
        Root,
        AwaitingTableNameKey,
        AwaitingRowListArray,
        AwaitingRow,
        AwaitingColumnNameKey,
        AwaitingColumnValue
    };

    using ColumnValue = std::variant<std::monostate, int64_t, double, std::string>;

    struct BuildingRow {
        std::stringstream query;
        std::vector<ColumnValue> values;
    };

    static void bindValue(sqlite::Statement &statement, int index, const std::monostate &value);

    template<typename T>
    static void bindValue(sqlite::Statement &statement, int index, const T &value);

    sqlite::Database &m_db;
    State m_state;
    std::optional<BuildingRow> m_row;
    std::optional<std::string> m_tableName;
};

#endif

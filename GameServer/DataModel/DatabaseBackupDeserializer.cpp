#include "DataModel/DatabaseBackupDeserializer.h"
#include "DataModel/DatabaseJSONRepresentation.h"
#include "DataModel/Sqlite/Database.h"
#include "DataModel/Sqlite/Statement.h"

DatabaseBackupDeserializer::DatabaseBackupDeserializer(sqlite::Database &db) : m_db(db), m_state(State::Root) {

}

DatabaseBackupDeserializer::~DatabaseBackupDeserializer() = default;

template<typename T, typename... Args>
bool DatabaseBackupDeserializer::storeValue(const Args &...args) {
    if(m_state != State::AwaitingColumnValue) {
        return false;
    }

    m_row.value().values.emplace_back(std::in_place_type_t<T>(), args...);
    m_state = State::AwaitingColumnNameKey;

    return true;
}

bool DatabaseBackupDeserializer::null() {
    return storeValue<std::monostate>();
}

bool DatabaseBackupDeserializer::boolean(bool value) {
    return storeValue<int64_t>(value);
}

bool DatabaseBackupDeserializer::integer(int64_t value) {
    return storeValue<int64_t>(value);
}

bool DatabaseBackupDeserializer::doubleValue(double value) {
    return storeValue<double>(value);
}

bool DatabaseBackupDeserializer::string(const std::string_view &value) {
    return storeValue<std::string>(value);
}

bool DatabaseBackupDeserializer::startMap() {
    if(m_state == State::Root) {
        m_state = State::AwaitingTableNameKey;

        return true;

    } else if(m_state == State::AwaitingRow) {
        m_row.emplace().query << "INSERT INTO " << m_tableName.value() << " (";
        m_state = State::AwaitingColumnNameKey;

        return true;
    } else {
        return false;
    }
}

bool DatabaseBackupDeserializer::mapKey(const std::string_view &key) {
    if(m_state == State::AwaitingTableNameKey) {
        m_tableName.emplace(entityNameToTableNameChecked(key));
        m_state = State::AwaitingRowListArray;

        return true;
    } else if(m_state == State::AwaitingColumnNameKey) {

        auto &row = m_row.value();

        if(!row.values.empty())
            row.query << ", ";

        row.query << entityFieldNameToColumnName(key);

        m_state = State::AwaitingColumnValue;

        return true;
    } else {
        return false;
    }
}

bool DatabaseBackupDeserializer::endMap() {
    if(m_state == State::AwaitingTableNameKey) {
        m_tableName.reset();
        m_state = State::Root;

        return true;
    } else if(m_state == State::AwaitingColumnNameKey) {
        auto &row = m_row.value();

        bool firstColumn = true;

        row.query << ") VALUES (";

        for(const auto &value: row.values) {
            if(firstColumn)
                firstColumn = false;
            else
                row.query << ", ";

            row.query << "?";
        }

        row.query << ")";

        auto statement = m_db.prepare(row.query.str());

        int index = 1;

        for(const auto &value: row.values) {
            std::visit([index, &statement](const auto &val) { bindValue(*statement, index, val); }, value);
            index += 1;
        }

        statement->exec();

        m_row.reset();

        m_state = State::AwaitingRow;

        return true;
    } else {
        return false;
    }
}

bool DatabaseBackupDeserializer::startArray(){
    if(m_state == State::AwaitingRowListArray) {
        m_state = State::AwaitingRow;
        return true;
    } else {
        return false;
    }
}

bool DatabaseBackupDeserializer::endArray(){
    if(m_state == State::AwaitingRow) {
        m_tableName.reset();
        m_state = State::AwaitingTableNameKey;
        return true;
    } else {
        return false;
    }
}

void DatabaseBackupDeserializer::bindValue(sqlite::Statement &statement, int index, const std::monostate &value) {
    statement.bindNull(index);
}

template<typename T>
void DatabaseBackupDeserializer::bindValue(sqlite::Statement &statement, int index, const T &value) {
    statement.bind(index, value);
}

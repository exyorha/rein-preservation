#ifndef DATA_MODEL_DATABASE_JSON_REPRESENTATION_H
#define DATA_MODEL_DATABASE_JSON_REPRESENTATION_H

#include <sqlite3.h>
#include <string_view>
#include <string>

namespace sqlite {
    class Statement;
}

namespace LLServices {
    class JSONWriter;
}

void writeSQLiteColumnValue(LLServices::JSONWriter &writer, sqlite::Statement &statement, int columnIndex);

void writeSQLiteValue(LLServices::JSONWriter &writer, sqlite3_value *value, bool isBoolean = false);

std::string tableNameToEntityName(const std::string_view &name);
std::string columnNameToEntityFieldName(const std::string_view &name);
std::string entityNameToTableNameChecked(const std::string_view &entityName);
std::string entityFieldNameToColumnName(const std::string_view &name);

#endif

#include <DataModel/DatabaseJSONRepresentation.h>
#include <DataModel/Sqlite/Statement.h>

#include <cstring>

#include "JSONWriter.h"

static std::string internalCapitalize(const std::string_view &name, bool capitalizeFirst) {
    bool capitalizeNext = capitalizeFirst;

    std::string output;
    output.reserve(name.size());

    for(auto ch: name) {
        if(capitalizeNext && ch >= 'a' && ch <= 'z') {
            ch = ch - 'a' + 'A';
        }

        if(ch == '_') {
            capitalizeNext = true;
        } else {
            capitalizeNext = false;

            output.push_back(ch);
        }
    }

    return output;
}

std::string tableNameToEntityName(const std::string_view &name) {
    return internalCapitalize(name, true);
}

std::string columnNameToEntityFieldName(const std::string_view &name) {
    return internalCapitalize(name, false);
}

std::string entityNameToTableNameChecked(const std::string_view &entityName) {
    std::string output;
    output.reserve(entityName.size() + entityName.size() / 4);

    for(auto ch: entityName) {
        if(ch >= 'A' && ch <= 'Z') {
            if(!output.empty())
                output.push_back('_');

            output.push_back(ch - 'A' + 'a');
        } else if(ch >= 'a' && ch <= 'z') {
            output.push_back(ch);
        } else {
            throw std::runtime_error("unacceptable characters in the entity name");
        }
    }

    if(!output.starts_with("i_"))
        throw std::runtime_error("attempted to access a non-individual table");

    return output;
}

void writeSQLiteColumnValue(JSONWriter &writer, sqlite::Statement &statement, int columnIndex) {
    auto type = statement.columnType(columnIndex);

    switch(type) {
        case SQLITE_INTEGER:
        {
            auto value = statement.columnInt64(columnIndex);

            auto declType = statement.columnDeclType(columnIndex);
            if(declType && strcmp(declType, "boolean") == 0) {
                writer.writeBool(value != 0);
            } else {
                writer.writeInteger(value);
            }

            break;
        }

        case SQLITE_FLOAT:
            writer.writeDouble(statement.columnDouble(columnIndex));
            break;

        case SQLITE_BLOB:
            throw std::runtime_error("attempted to represent a BLOB in JSON");

        case SQLITE_NULL:
            writer.writeNull();
            break;

        case SQLITE_TEXT:
            writer.writeString(std::string_view(statement.columnText(columnIndex), statement.columnBytes(columnIndex)));
            break;

        default:
            throw std::runtime_error("bad SQLite column type");
    }
}

void writeSQLiteValue(JSONWriter &writer, sqlite3_value *value, bool isBoolean) {
    switch(sqlite3_value_type(value)) {
        case SQLITE_INTEGER:
        {
            auto intValue = sqlite3_value_int64(value);

            if(isBoolean) {
                writer.writeBool(intValue != 0);
            } else {
                writer.writeInteger(intValue);
            }

            break;
        }

        case SQLITE_FLOAT:
            writer.writeDouble(sqlite3_value_double(value));
            break;

        case SQLITE_BLOB:
            throw std::runtime_error("attempted to represent a BLOB in JSON");

        case SQLITE_NULL:
            writer.writeNull();
            break;

        case SQLITE_TEXT:
            writer.writeString(std::string_view(reinterpret_cast<const char *>(sqlite3_value_text(value)), sqlite3_value_bytes(value)));
            break;

        default:
            throw std::runtime_error("bad SQLite column type");
    }
}

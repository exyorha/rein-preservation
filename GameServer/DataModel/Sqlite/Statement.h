#ifndef DATA_MODEL_SQLITE_STATEMENT_H
#define DATA_MODEL_SQLITE_STATEMENT_H

#include <sqlite3.h>

#include <memory>
#include <string>
#include <vector>

#include "Value.h"

namespace sqlite {
    class Statement {
        friend class Database;

        Statement(sqlite3 *owningContext);
        void acceptHandle(sqlite3_stmt *stmt);

    public:
        ~Statement();

        Statement(const Statement &other) = delete;
        Statement &operator =(const Statement &other) = delete;

        inline void reset() {
            sqlite3_reset(m_handle);
        }

        bool step();
        void exec();

        inline int bindParameterCount() const { return sqlite3_bind_parameter_count(m_handle); }
        inline int bindParameterIndex(const char *parameter) const { return sqlite3_bind_parameter_index(m_handle, parameter); }
        inline const char *bindParameterName(int index) const { return sqlite3_bind_parameter_name(m_handle, index); }

        inline void clearBindings() { sqlite3_clear_bindings(m_handle); }
        void bind(int index, const std::vector<unsigned char> &data);
        void bind(int index, double value);
        void bind(int index, int value);
        void bind(int index, int64_t value);
        void bindNull(int index);
        void bind(int index, const std::string_view &data);
        void bind(int index, const std::wstring &data);
        void bind(int index, const Value &value);
        void bindZeroBlob(int index, sqlite3_int64 length);

        inline const void *columnBlob(int column) { return sqlite3_column_blob(m_handle, column); }
        inline int columnBytes(int column) const { return sqlite3_column_bytes(m_handle, column); }
        inline double columnDouble(int column) const { return sqlite3_column_double(m_handle, column); };
        inline int columnInt(int column) const { return sqlite3_column_int(m_handle, column); }
        inline int64_t columnInt64(int column) const { return sqlite3_column_int64(m_handle, column); }
        inline const char *columnText(int column) const { return (const char *)sqlite3_column_text(m_handle, column); }
        inline int columnType(int column) const { return sqlite3_column_type(m_handle, column); }
        inline Value column(int column) const { return sqlite3_column_value(m_handle, column); }

        inline const char *columnName(int column) const { return sqlite3_column_name(m_handle, column); }
        inline const char *columnDeclType(int column) const { return sqlite3_column_decltype(m_handle, column); }

        inline std::string sql() const { return sqlite3_sql(m_handle); }
        inline bool isBusy() const { return sqlite3_stmt_busy(m_handle) != 0; }
        inline bool isReadOnly() const { return sqlite3_stmt_readonly(m_handle) != 0; }

        inline int columnCount() const { return sqlite3_column_count(m_handle); }
        inline int dataCount() const { return sqlite3_data_count(m_handle); }

    private:
        sqlite3 *m_owningContext;
        sqlite3_stmt *m_handle;
    };
}

#endif

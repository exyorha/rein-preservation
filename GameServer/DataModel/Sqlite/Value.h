#ifndef DATA_MODEL_SQLITE_VALUE_H
#define DATA_MODEL_SQLITE_VALUE_H

#include <sqlite3.h>

namespace sqlite {
    class Value {
    public:
        inline Value(sqlite3_value *value, bool fromDup = false) : m_handle(value), m_fromDup(fromDup) { }
        ~Value() noexcept;

        Value(Value &&other) noexcept;

        Value(const Value &other) = delete;
        Value &operator =(const Value &other) = delete;

        inline sqlite3_value *handle() const { return m_handle; }
        operator sqlite3_value *() const { return m_handle; }

        inline const void *blob() const { return sqlite3_value_blob(m_handle); }
        inline int bytes() const { return sqlite3_value_bytes(m_handle); }
        inline int bytes16() const { return sqlite3_value_bytes16(m_handle); }
        inline double asDouble() const {
            return sqlite3_value_double(m_handle);
        }
        inline int asInt() const {
            return sqlite3_value_int(m_handle);
        }
        inline sqlite3_int64 asInt64() const {
            return sqlite3_value_int64(m_handle);
        }
        inline const unsigned char *text() const {
            return sqlite3_value_text(m_handle);
        }
        inline const void *text16() const {
            return sqlite3_value_text16(m_handle);
        }
        inline const void *text16le() const {
            return sqlite3_value_text16le(m_handle);
        }
        inline const void *text16be() const {
            return sqlite3_value_text16be(m_handle);
        }

        inline int type() const { return sqlite3_value_type(m_handle); }
        inline int numericType() const { return sqlite3_value_numeric_type(m_handle); }
        inline unsigned int subtype() const { return sqlite3_value_subtype(m_handle); }

        Value dup();

    private:
        sqlite3_value *m_handle;
        bool m_fromDup;
    };
}

#endif

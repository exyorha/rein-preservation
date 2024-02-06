#include <DataModel/Sqlite/Value.h>

#include <stdexcept>
#include <new>

namespace sqlite {

    Value::Value(Value &&other) noexcept {
        m_handle = other.m_handle;
        m_fromDup = other.m_fromDup;

        other.m_handle = nullptr;
        other.m_fromDup = false;
    }

    Value::~Value() noexcept {
        if (m_fromDup) {
            sqlite3_value_free(m_handle);
        }
    }

    Value Value::dup() {
        auto rawHandle = sqlite3_value_dup(m_handle);
        if (!rawHandle)
            throw std::bad_alloc();

        return Value(rawHandle, true);
    }
}

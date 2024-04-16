#include <DataModel/Sqlite/Statement.h>
#include <DataModel/Sqlite/Error.h>
#include <DataModel/Sqlite/Value.h>

namespace sqlite {

    Statement::Statement(sqlite3 *owningContext) : m_owningContext(owningContext), m_handle(nullptr) {

    }

    void Statement::acceptHandle(sqlite3_stmt *stmt) {
        sqlite3_finalize(m_handle);
        m_handle = stmt;
    }

    Statement::~Statement() {
        sqlite3_finalize(m_handle);
        m_handle = nullptr;
    }

    bool Statement::step() {
        int status = sqlite3_step(m_handle);

        if (status == SQLITE_DONE)
            return false;
        else if (status == SQLITE_ROW)
            return true;

        Error::raiseError(status, m_owningContext);
    }

    void Statement::exec() {
        while(step());

        reset();
    }

    void Statement::bind(int index, const std::vector<unsigned char> &data) {
        int status = sqlite3_bind_blob64(m_handle, index, &data[0], data.size(), SQLITE_TRANSIENT);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, const unsigned char *data, size_t dataSize) {
        int status = sqlite3_bind_blob64(m_handle, index, data, dataSize, SQLITE_TRANSIENT);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, double value) {
        int status = sqlite3_bind_double(m_handle, index, value);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, int value) {
        int status = sqlite3_bind_int(m_handle, index, value);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, int64_t value) {
        int status = sqlite3_bind_int64(m_handle, index, value);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bindNull(int index) {
        int status = sqlite3_bind_null(m_handle, index);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, const std::string_view &data) {
        int status = sqlite3_bind_text64(m_handle, index, data.data(), data.size(), SQLITE_TRANSIENT, SQLITE_UTF8);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, const std::wstring &data) {
        int status = sqlite3_bind_text64(m_handle, index, (const char *)data.data(), data.size() * sizeof(wchar_t), SQLITE_TRANSIENT, SQLITE_UTF16);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bind(int index, const Value &value) {
        int status = sqlite3_bind_value(m_handle, index, value.handle());
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Statement::bindZeroBlob(int index, sqlite3_int64 length) {
        int status = sqlite3_bind_zeroblob64(m_handle, index, length);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }
}

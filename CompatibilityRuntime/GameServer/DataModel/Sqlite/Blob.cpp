#include <DataModel/Sqlite/Blob.h>
#include <DataModel/Sqlite/Error.h>

namespace sqlite {

    Blob::Blob(sqlite3 *owningContext) : m_owningContext(owningContext), m_handle(nullptr) {

    }

    void Blob::acceptHandle(sqlite3_blob *blob) {
        int status = sqlite3_blob_close(m_handle);
        m_handle = nullptr;

        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);

        m_handle = blob;
    }

    Blob::~Blob() {
        int status = sqlite3_blob_close(m_handle);
        m_handle = nullptr;

        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    void Blob::reopen(sqlite3_int64 row) {
        int status = sqlite3_blob_reopen(m_handle, row);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

    int Blob::bytes() const {
        return sqlite3_blob_bytes(m_handle);
    }

    void Blob::read(void *buffer, int size, int offset) {
        int status = sqlite3_blob_read(m_handle, buffer, size, offset);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);

    }

    void Blob::write(const void *buffer, int size, int offset) {
        int status = sqlite3_blob_write(m_handle, buffer, size, offset);
        if (status != SQLITE_OK)
            Error::raiseError(status, m_owningContext);
    }

}

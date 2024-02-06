#ifndef DATA_MODEL_SQLITE_BLOB_H
#define DATA_MODEL_SQLITE_BLOB_H

#include <sqlite3.h>

#include <memory>

namespace sqlite {
    class Blob {
        friend class Database;

        Blob(sqlite3 *owningContext);
        void acceptHandle(sqlite3_blob *blob);

    public:
        ~Blob();

        Blob(const Blob &other) = delete;
        Blob &operator =(const Blob &other) = delete;

        void reopen(sqlite3_int64 row);

        int bytes() const;
        void read(void *buffer, int size, int offset);
        void write(const void *buffer, int size, int offset);

    private:
        sqlite3 *m_owningContext;
        sqlite3_blob *m_handle;
    };
}

#endif

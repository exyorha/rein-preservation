#ifndef MASTER_DATABASE_CURSOR_H
#define MASTER_DATABASE_CURSOR_H

#include <sqlite3ext.h>

#include <cstring>
#include <memory>

namespace MasterDatabase {

    class MasterDatabaseTable;
    class DatabaseTable;

    class MasterDatabaseCursor final : public sqlite3_vtab_cursor {
    public:
        explicit MasterDatabaseCursor(MasterDatabaseTable *table);
        ~MasterDatabaseCursor();

        MasterDatabaseCursor(const MasterDatabaseCursor &other) = delete;
        MasterDatabaseCursor &operator =(const MasterDatabaseCursor &other) = delete;

        inline bool atEof() const noexcept {
            return m_pos >= m_end;
        }

        void filter(int idxNum, const char *idxStr, int argc, sqlite3_value **argv);

        void next();

        void column(sqlite3_context *ctx, int i) const;
        sqlite3_int64 rowid() const;

    private:
        std::shared_ptr<DatabaseTable> m_table;

        size_t m_pos;
        size_t m_end;
    };

}

#endif

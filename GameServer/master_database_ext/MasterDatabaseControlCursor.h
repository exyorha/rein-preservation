#ifndef MASTER_DATABASE_CONTROL_CURSOR_H
#define MASTER_DATABASE_CONTROL_CURSOR_H

#include <sqlite3ext.h>

#include <cstring>

namespace MasterDatabase {

    class MasterDatabaseControlTable;

    class MasterDatabaseControlCursor final : public sqlite3_vtab_cursor {
    public:
        explicit MasterDatabaseControlCursor(MasterDatabaseControlTable *table);
        ~MasterDatabaseControlCursor();

        MasterDatabaseControlCursor(const MasterDatabaseControlCursor &other) = delete;
        MasterDatabaseControlCursor &operator =(const MasterDatabaseControlCursor &other) = delete;

        inline bool atEof() const noexcept {
            return m_pos >= m_end;
        }

        void filter(int idxNum, const char *idxStr, int argc, sqlite3_value **argv);

        void next();

        void column(sqlite3_context *ctx, int i) const;
        sqlite3_int64 rowid() const;

    private:
        MasterDatabaseControlTable *m_table;
        size_t m_pos;
        size_t m_end;
    };

}

#endif

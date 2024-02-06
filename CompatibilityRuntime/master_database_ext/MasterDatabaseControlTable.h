#ifndef MASTER_DATABASE_CONTROL_TABLE_H
#define MASTER_DATABASE_CONTROL_TABLE_H

#include <sqlite3ext.h>

#include <string>

namespace MasterDatabase {

    class GlobalContext;

    class MasterDatabaseControlTable final : public sqlite3_vtab {
    public:
        MasterDatabaseControlTable(GlobalContext *context, sqlite3 *db);
        ~MasterDatabaseControlTable();

        MasterDatabaseControlTable(const MasterDatabaseControlTable &other) = delete;
        MasterDatabaseControlTable &operator =(const MasterDatabaseControlTable &other) = delete;

        inline GlobalContext *context() const {
            return m_ctx;
        }

        void bestIndex(sqlite3_index_info *indexInfo);

        sqlite3_int64 update(int argc, sqlite3_value **argv);

    private:
        GlobalContext *m_ctx;
    };

}

#endif

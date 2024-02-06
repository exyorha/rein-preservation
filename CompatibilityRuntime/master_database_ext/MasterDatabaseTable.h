#ifndef MASTER_DATABASE_TABLE_H
#define MASTER_DATABASE_TABLE_H

#include <sqlite3ext.h>

#include <string>
#include <memory>

namespace MasterDatabase {

    class GlobalContext;
    class DatabaseTable;

    class MasterDatabaseTable final : public sqlite3_vtab {
    public:
        MasterDatabaseTable(GlobalContext *context, sqlite3 *db, int argc, const char *const*argv);
        ~MasterDatabaseTable();

        MasterDatabaseTable(const MasterDatabaseTable &other) = delete;
        MasterDatabaseTable &operator =(const MasterDatabaseTable &other) = delete;

        void bestIndex(sqlite3_index_info *indexInfo);

        std::shared_ptr<DatabaseTable> open();

    private:
        GlobalContext *m_context;
        std::string m_tableName;
        std::weak_ptr<DatabaseTable> m_cachedTable;
    };

}

#endif

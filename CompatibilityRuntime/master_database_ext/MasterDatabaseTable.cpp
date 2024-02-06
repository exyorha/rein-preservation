#include "MasterDatabaseTable.h"
#include "DatabaseTable.h"
#include "GlobalContext.h"

#include <stdexcept>
#include <sstream>

SQLITE_EXTENSION_INIT3

namespace MasterDatabase {

    MasterDatabaseTable::MasterDatabaseTable(GlobalContext *context, sqlite3 *db, int argc, const char *const*argv) : m_context(context) {
        sqlite3_vtab_config(db, SQLITE_VTAB_INNOCUOUS);

        if(argc < 3)
            throw std::logic_error("MasterDatabaseTable: not enough parameters");

        m_tableName = argv[2];
        std::stringstream tableSpec;

        tableSpec << "CREATE TABLE " << m_tableName << " (\n";

        for(int arg = 3; arg < argc; arg++) {
            tableSpec << "    " << argv[arg];

            if(arg < argc - 1)
                tableSpec << ",";

            tableSpec << "\n";
        }

        tableSpec << ")\n";

        auto result = sqlite3_declare_vtab(db, tableSpec.str().c_str());
        if(result != SQLITE_OK)
            throw std::runtime_error("sqlite3_declare_vtab has failed");
    }

    MasterDatabaseTable::~MasterDatabaseTable() = default;

    void MasterDatabaseTable::bestIndex(sqlite3_index_info *indexInfo) {
        /*
         * No indices yet.
         */

        indexInfo->estimatedCost = 10.0;
        indexInfo->estimatedRows = 10;
    }

    std::shared_ptr<DatabaseTable> MasterDatabaseTable::open() {
        auto table = m_cachedTable.lock();

        if(table) {
            return table;
        }

        table = m_context->open(m_tableName);
        m_cachedTable = table;

        return table;
    }
}



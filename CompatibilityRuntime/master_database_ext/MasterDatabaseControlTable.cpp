#include "MasterDatabaseControlTable.h"
#include "GlobalContext.h"

#include <algorithm>
#include <stdexcept>
#include <sstream>

SQLITE_EXTENSION_INIT3

namespace MasterDatabase {

    MasterDatabaseControlTable::MasterDatabaseControlTable(GlobalContext *context, sqlite3 *db) : m_ctx(context) {
        sqlite3_vtab_config(db, SQLITE_VTAB_DIRECTONLY);

        auto result = sqlite3_declare_vtab(db, "CREATE TABLE tbl (database_filename TEXT)");
        if(result != SQLITE_OK)
            throw std::runtime_error("sqlite3_declare_vtab has failed");
    }

    MasterDatabaseControlTable::~MasterDatabaseControlTable() = default;

    void MasterDatabaseControlTable::bestIndex(sqlite3_index_info *indexInfo) {
        indexInfo->estimatedCost = 10.0;
        indexInfo->estimatedRows = 1;
    }

    sqlite3_int64 MasterDatabaseControlTable::update(int argc, sqlite3_value **argv) {
        if(argc < 1) {
            throw std::logic_error("MasterDatabaseControlTable::update: not enough arguments");
        }

        if(argv[0] == nullptr) {
            // INSERT: 'set path'

        } else {
            auto rowId = sqlite3_value_int64(argv[0]);
            if(rowId != 0) {
                throw std::logic_error("MasterDatabaseControlTable::update: bad row ID to update");
            }
        }

        if(argc == 1) {
            // DELETE = 'set path to empty'

            m_ctx->setDatabaseFile(std::filesystem::path());
        } else if(argc < 3) {
            throw std::logic_error("MasterDatabaseControlTable::update: not enough arguments");
        } else {
            // INSERT or UPDATE: set path

            auto path = sqlite3_value_text(argv[2]);
            auto pathLength = sqlite3_value_bytes(argv[2]);

            std::filesystem::path fsPath;
            if(path) {
                // NULL = empty path, 'clear'

                fsPath.assign(std::u8string(reinterpret_cast<const char8_t *>(path), pathLength));
            }

            m_ctx->setDatabaseFile(std::move(fsPath));
        }

        return 0;
    }
}



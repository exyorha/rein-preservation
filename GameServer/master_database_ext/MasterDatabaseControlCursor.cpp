#include "MasterDatabaseControlCursor.h"
#include "MasterDatabaseControlTable.h"
#include "GlobalContext.h"

#include <stdexcept>

SQLITE_EXTENSION_INIT3

namespace MasterDatabase {

    MasterDatabaseControlCursor::MasterDatabaseControlCursor(MasterDatabaseControlTable *table) : m_table(table), m_pos(0), m_end(1) {

    }

    MasterDatabaseControlCursor::~MasterDatabaseControlCursor() = default;

    void MasterDatabaseControlCursor::filter(int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
        m_pos = 0;
        m_end = 1;
    }

    void MasterDatabaseControlCursor::next() {
        if(atEof())
            throw std::logic_error("the cursor is at the end\n");

        m_pos++;
    }

    void MasterDatabaseControlCursor::column(sqlite3_context *ctx, int i) const {
        if(atEof())
            throw std::logic_error("the cursor is at the end\n");

        if(i > 0)
            throw std::logic_error("column index is out of range");

        if(i == 0) {
            auto string = m_table->context()->databaseFile().u8string();

            sqlite3_result_text(ctx, reinterpret_cast<const char *>(string.data()), string.size(), SQLITE_TRANSIENT);
        }
    }

    sqlite3_int64 MasterDatabaseControlCursor::rowid() const {
        if(atEof())
            throw std::logic_error("the cursor is at the end\n");

        return m_pos;
    }
}

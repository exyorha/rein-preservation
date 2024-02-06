#include "MasterDatabaseCursor.h"
#include "MasterDatabaseTable.h"

#include "DatabaseTable.h"
#include "msgpack/v3/object_fwd_decl.hpp"

#include <stdexcept>

SQLITE_EXTENSION_INIT3

namespace MasterDatabase {

    MasterDatabaseCursor::MasterDatabaseCursor(MasterDatabaseTable *table) : m_table(table->open()) {

    }

    MasterDatabaseCursor::~MasterDatabaseCursor() = default;

    void MasterDatabaseCursor::filter(int idxNum, const char *idxStr, int argc, sqlite3_value **argv) {
        m_pos = 0;
        m_end = 0;

        if(m_table) {
            m_end = m_table->size();
        }
    }

    void MasterDatabaseCursor::next() {
        if(atEof())
            throw std::logic_error("the cursor is at the end");

        m_pos++;
    }

    void MasterDatabaseCursor::column(sqlite3_context *ctx, int i) const {
        if(atEof())
            throw std::logic_error("the cursor is at the end");

        const auto &row = m_table->row(m_pos);
        if(row.type != msgpack::type::ARRAY)
            throw std::runtime_error("the row is not an array");

        const auto &rowArray = row.via.array;
        if(i >= rowArray.size) {
            sqlite3_result_null(ctx);

            return;
        }

        const auto &column = rowArray.ptr[i];

        switch(column.type) {
            case msgpack::type::NIL:
                sqlite3_result_null(ctx);
                break;

            case msgpack::type::BOOLEAN:
                sqlite3_result_int(ctx, column.via.boolean);
                break;

            case msgpack::type::POSITIVE_INTEGER:
            case msgpack::type::NEGATIVE_INTEGER:
                sqlite3_result_int64(ctx, column.via.i64);
                break;

            case msgpack::type::FLOAT32:
            case msgpack::type::FLOAT64:
                sqlite3_result_double(ctx, column.via.f64);
                break;

            case msgpack::type::STR:
                sqlite3_result_text(ctx, column.via.str.ptr, column.via.str.size, SQLITE_STATIC);
                break;

            case msgpack::type::BIN:
                sqlite3_result_blob64(ctx, column.via.bin.ptr, column.via.bin.size, SQLITE_STATIC);
                break;

            default:
                throw std::runtime_error("unrepresentable column value");
        }
    }

    sqlite3_int64 MasterDatabaseCursor::rowid() const {
        if(atEof())
            throw std::logic_error("the cursor is at the end\n");

        return m_pos;
    }
}

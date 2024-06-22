#include <DataModel/ChangesetCapturingScope.h>
#include <DataModel/TableChangesetWriter.h>

#include <DataModel/Sqlite/Database.h>

#include <sqlite3.h>

#include <cstring>
#include <optional>

ChangesetCapturingScope::ChangesetCapturingScope(sqlite::Database &database) : m_database(database), m_session(database, "main") {
    sqlite3session_table_filter(m_session, tableFilter, this);
}

ChangesetCapturingScope::~ChangesetCapturingScope() = default;

int ChangesetCapturingScope::tableFilter(void *context, const char *tableName) {
    (void)context;

    return strncmp(tableName, "i_", 2) == 0;
}

void ChangesetCapturingScope::serialize(::google::protobuf::Map<std::string, ::apb::api::data::DiffData> *rpcChangeset) {
    auto changeset = m_session.changeset();

    std::optional<TableChangesetWriter> writer;

    sqlite::ChangesetIterator it(changeset);
    sqlite::ChangesetOperation op;
    while(it.next(op)) {
        if(!writer.has_value() || !writer->isSameTable(op.tableName)) {
            if(writer.has_value()) {
                writer->finalize();
            }

            writer.emplace(m_database, rpcChangeset, op.tableName);
        }

        writer->writeEntry(it, op);
    }

    if(writer.has_value())
        writer->finalize();
}


#include "DatabaseViewer/DatabaseViewerSQL.h"

#include "DataModel/Database.h"
#include "DataModel/Sqlite/Transaction.h"
#include "DataModel/Sqlite/Statement.h"

DatabaseViewerSQL::DatabaseViewerSQL(std::filesystem::path &&filename, Database &db) : ReloadableResource(std::move(filename)),
    m_db(db) {

}

DatabaseViewerSQL::~DatabaseViewerSQL() = default;

void DatabaseViewerSQL::doReload(std::string &&content) {
    sqlite::Transaction transaction(&m_db.db());

    auto getExistingTables = m_db.db().prepare(
        "SELECT name FROM pragma_table_list WHERE schema = 'temp' AND type = 'view' AND name LIKE 'dbview\\_%' ESCAPE '\\'"
    );

    while(getExistingTables->step()) {
        std::string name = getExistingTables->columnText(0);

        auto dropTable = m_db.db().prepare("DROP VIEW " + name);
        dropTable->exec();
    }
    getExistingTables->reset();


    const char* query = content.c_str();
    while (*query) {
        auto statement = m_db.db().prepare(query, 0, &query);
        if (statement)
            statement->exec();
    }

    transaction.commit();
}

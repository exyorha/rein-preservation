#include "DatabaseViewer/DatabaseTextImporter.h"

#include "DataModel/Sqlite/Database.h"
#include "DataModel/Sqlite/Statement.h"

DatabaseTextImporter::DatabaseTextImporter(sqlite::Database &db) :
    m_statement(db.prepare("INSERT OR IGNORE INTO database_viewer_text (language, message_key, message) VALUES (?, ?, ?)")) {

}

DatabaseTextImporter::~DatabaseTextImporter() = default;

void DatabaseTextImporter::addTextEntry(const std::string_view &language, const std::string_view &key, std::string &&value) {
    m_statement->bind(1, language);
    m_statement->bind(2, key);
    m_statement->bind(3, value);
    m_statement->exec();
    m_statement->reset();
    m_statement->clearBindings();
}

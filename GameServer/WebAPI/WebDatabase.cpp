#include "WebAPI/WebDatabase.h"
#include "ReloadableResource.h"

#include "DataModel/Sqlite/Database.h"
#include "DataModel/Sqlite/Statement.h"
#include "DataModel/Database.h"

WebDatabase::WebDatabase(std::filesystem::path &&path, Database &db) : ReloadableResource(std::move(path)), m_db(db) {

}

WebDatabase::~WebDatabase() = default;

void WebDatabase::doReload(std::string &&content) {
    static const char *webDatabase = "file:web?mode=memory&cache=shared";

    try {
        m_db.db().prepare("DETACH DATABASE web")->exec();
    } catch(...) {}

    sqlite::Database db(webDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI);

    const char *query = content.c_str();

    while(*query) {

        while(isspace(*query))
            query++;

        if(!*query)
            break;

        db.prepare(query, 0, &query)->exec();
    }

    auto attach = m_db.db().prepare("ATTACH DATABASE ? AS web");
    attach->bind(1, webDatabase);
    attach->exec();
}

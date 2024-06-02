#include "DatabaseViewer/DatabaseViewerService.h"
#include "DataModel/Sqlite/Transaction.h"
#include "DatabaseViewer/DatabaseViewerResponse.h"
#include "DatabaseViewer/HierarchicalPathParser.h"
#include "DatabaseViewer/DatabaseTextImporter.h"

#include "DataModel/Database.h"
#include "DataModel/Sqlite/Statement.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Logging/LogFacility.h"


#include <gitversion.h>

#include <ClientDataAccess/OctoContentStorage.h>

LLServices::LogFacility LogDatabaseViewerService("DatabaseViewerService");

DatabaseViewerService::DatabaseViewerService(
    const ClientDataAccess::OctoContentStorage &contentStorage,
    Database &db,
    const std::filesystem::path &dataPath
) :
    m_contentStorage(contentStorage),
    m_db(db),
    m_schema(dataPath / "dbview_schema.json"),
    m_sql(dataPath / "dbview_schema.sql", m_db),
    m_textAssetsLoaded(false) {

}

DatabaseViewerService::~DatabaseViewerService() = default;

void DatabaseViewerService::handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) {

    if(request.command() != EVHTTP_REQ_GET) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    m_schema.reloadIfNeeded();
    m_sql.reloadIfNeeded();

    if(!m_textAssetsLoaded) {
        LogDatabaseViewerService.info("Loading the text assets from the game data");

        m_db.setQueryLoggingEnabled(false);

        {
            sqlite::Transaction transaction(&m_db.db());

            DatabaseTextImporter importer(m_db.db());
            importer.parseAllTextAssets(m_contentStorage);

            transaction.commit();
        }

        m_db.setQueryLoggingEnabled(true);

        LogDatabaseViewerService.info("Finished loading");

        m_textAssetsLoaded = true;
    }

    DatabaseViewerResponse response(std::move(request));

    response.root().attributes().emplace_back("octo-version", std::to_string(m_contentStorage.database().revision()));
    response.root().attributes().emplace_back("master-db-version", m_db.masterDatabaseVersion());

#if defined(PROJECT_GITVERSION)
    response.root().attributes().emplace_back("gitversion", PROJECT_GITVERSION);
#endif

    auto &entities = response.root().children().emplace_back("entities");
    for(const auto &entity: m_schema.entities().entities()) {
        auto &entityNode = entities.children().emplace_back("entity");
        entityNode.attributes().emplace_back("name", entity.entityName());
    }

    HierarchicalPathParser pathParser(routedPath);
    if(pathParser.atEnd()) {
        /*
         * Index page
         */
    } else {
        auto entityName = pathParser.getElement();

        auto entity = m_schema.entities().findEntityByName(entityName);
        if(entity == nullptr) {
            request.sendError(404, "Not Found");
            return;
        }

        /*
         * Entity table page
         */

        auto &entityList = response.root().children().emplace_back("entity-list");
        entityList.attributes().emplace_back("name", entity->entityName());

        auto &columns = entityList.children().emplace_back("columns");

        auto getColumns = m_db.db().prepare("SELECT name FROM pragma_table_info(?)");
        getColumns->bind(1, "dbview_" + entity->entityName());
        while(getColumns->step()) {
            auto &column = columns.children().emplace_back("column");
            column.attributes().emplace_back("name", getColumns->columnText(0));
        }
        getColumns->reset();

        auto &items = entityList.children().emplace_back("items");

        auto getValues = m_db.db().prepare("SELECT * FROM dbview_" + entity->entityName());
        while(getValues->step()) {
            auto &item = items.children().emplace_back("item");

            for(int colIndex = 0, colCount = getValues->dataCount(); colIndex < colCount; colIndex++) {
                auto &column = item.children().emplace_back("column");
                column.attributes().emplace_back("name", getValues->columnName(colIndex));

                auto content = getValues->columnText(colIndex);
                if(content)
                    column.setContent(content);
            }
        }
        getValues->reset();
    }

    /*
     * Get the server time at the *end* of the response processing for better
     * accuracy would the page rendering take a long time.
     */

    response.root().attributes().emplace_back("current-time", std::to_string(m_db.realWorldTime()));
    if(m_db.timeOffset().has_value()) {
        response.root().attributes().emplace_back("time-offset", std::to_string(*m_db.timeOffset()));
    }

    response.finish();
}

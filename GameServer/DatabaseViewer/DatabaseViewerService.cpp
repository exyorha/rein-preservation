#include "DatabaseViewer/DatabaseViewerService.h"
#include "DatabaseViewer/DatabaseViewerResponse.h"

#include "DataModel/Database.h"

#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/KeyValuePairs.h"

#include <gitversion.h>

DatabaseViewerService::DatabaseViewerService(int32_t octoVersion, Database &db) : m_octoVersion(octoVersion), m_db(db) {

}

DatabaseViewerService::~DatabaseViewerService() = default;

void DatabaseViewerService::handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) {

    if(request.command() != EVHTTP_REQ_GET) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    if(routedPath != "/") {
        request.sendError(404, "Not Found");
        return;
    }

    DatabaseViewerResponse response(std::move(request));

    response.root().attributes().emplace_back("octo-version", std::to_string(m_octoVersion));
    response.root().attributes().emplace_back("master-db-version", m_db.masterDatabaseVersion());

#if defined(SERVER_GITVERSION)
    response.root().attributes().emplace_back("gitversion", SERVER_GITVERSION);
#endif

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

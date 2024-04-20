#ifndef DATABASE_VIEWER_DATABASE_VIEWER_SERVICE_H
#define DATABASE_VIEWER_DATABASE_VIEWER_SERVICE_H

#include "WebServices/WebRoutable.h"

#include "DatabaseViewer/DatabaseViewerSchema.h"
#include "DatabaseViewer/DatabaseViewerSQL.h"

class Database;

namespace ClientDataAccess {
    class OctoContentStorage;
}

namespace LLServices {
    class LogFacility;
}

class DatabaseViewerService final : public WebRoutable {
public:
    DatabaseViewerService(const ClientDataAccess::OctoContentStorage &contentStorage, Database &db, const std::filesystem::path &dataPath);
    ~DatabaseViewerService();

    DatabaseViewerService(const DatabaseViewerService &other) = delete;
    DatabaseViewerService &operator =(const DatabaseViewerService &other) = delete;

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

private:
    const ClientDataAccess::OctoContentStorage &m_contentStorage;
    Database &m_db;
    DatabaseViewerSchema m_schema;
    DatabaseViewerSQL m_sql;
    bool m_textAssetsLoaded;
};

extern LLServices::LogFacility LogDatabaseViewerService;

#endif

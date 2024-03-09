#ifndef DATABASE_VIEWER_DATABASE_VIEWER_SERVICE_H
#define DATABASE_VIEWER_DATABASE_VIEWER_SERVICE_H

#include "WebServices/WebRoutable.h"

#include <cstdint>

class Database;

class DatabaseViewerService final : public WebRoutable {
public:
    DatabaseViewerService(int32_t octoVersion, Database &db);
    ~DatabaseViewerService();

    DatabaseViewerService(const DatabaseViewerService &other) = delete;
    DatabaseViewerService &operator =(const DatabaseViewerService &other) = delete;

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

private:
    int32_t m_octoVersion;
    Database &m_db;
};

#endif

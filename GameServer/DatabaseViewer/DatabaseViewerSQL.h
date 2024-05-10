#ifndef DATABASE_VIEWER_DATABASE_VIEWER_SQL_H
#define DATABASE_VIEWER_DATABASE_VIEWER_SQL_H

#include "ReloadableResource.h"

class Database;

class DatabaseViewerSQL final : public ReloadableResource {
public:
    DatabaseViewerSQL(std::filesystem::path &&filename, Database &db);
    ~DatabaseViewerSQL();

protected:
    void doReload(std::string &&content) override;

private:
    Database &m_db;
};

#endif

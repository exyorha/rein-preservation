#ifndef WEB_API_WEB_DATABASE_H
#define WEB_API_WEB_DATABASE_H

#include "ReloadableResource.h"

class Database;

class WebDatabase final : public ReloadableResource {
public:
    WebDatabase(std::filesystem::path &&path, Database &db);
    ~WebDatabase();

protected:
    void doReload(std::string &&content) override;

private:
    Database &m_db;
};

#endif


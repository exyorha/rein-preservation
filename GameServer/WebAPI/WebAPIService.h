#ifndef WEB_API_WEB_API_SERVICE_H
#define WEB_API_WEB_API_SERVICE_H

#include "LLServices/Networking/HttpRequest.h"

#include <WebServices/WebRoutable.h>

#include <WebAPI/WebDatabase.h>

namespace LLServices {
    class JSONTree;
    class JSONWriter;
}

class Database;

class WebAPIService final : public WebRoutable {
public:
    WebAPIService(Database &db, const std::filesystem::path &resourcePath);
    ~WebAPIService();

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;

private:
    struct PathHandler {
        std::string_view path;
        void (WebAPIService::*handler)(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response);
    };

    void informationListGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response);
    void informationDetailGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response);
    void bannerListGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response);

    Database &m_db;
    WebDatabase m_webDB;
    static const PathHandler m_pathHandlers[];
};

#endif

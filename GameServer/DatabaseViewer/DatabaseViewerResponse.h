#ifndef DATABASE_VIEWER_DATABASE_VIEWER_RESPONSE_H
#define DATABASE_VIEWER_DATABASE_VIEWER_RESPONSE_H

#include <DatabaseViewer/XMLFile.h>

#include <LLServices/Networking/HttpRequest.h>

class DatabaseViewerResponse {
public:
    explicit DatabaseViewerResponse(LLServices::HttpRequest &&request);
    ~DatabaseViewerResponse();

    DatabaseViewerResponse(const DatabaseViewerResponse &other) = delete;
    DatabaseViewerResponse &operator =(const DatabaseViewerResponse &other) = delete;

    DatabaseViewerResponse(DatabaseViewerResponse &&other) noexcept;
    DatabaseViewerResponse &operator =(DatabaseViewerResponse &&other) noexcept;

    inline XMLNode &root() {
        return m_response.root();
    }

    void finish();

private:
    XMLFile m_response;
    int32_t m_octoVersion;
    LLServices::HttpRequest m_request;
};

#endif


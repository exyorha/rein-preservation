#ifndef WEB_SERVICES_SERVER_CLI_SERVICE_H
#define WEB_SERVICES_SERVER_CLI_SERVICE_H

#include <WebServices/WebRoutable.h>

class ServerCLIService final : public WebRoutable {
public:
    ServerCLIService();
    ~ServerCLIService();

    void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) override;
};

#endif

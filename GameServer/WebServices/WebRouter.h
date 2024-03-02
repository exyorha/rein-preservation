#ifndef WEB_SERVICES_WEB_ROUTER_H
#define WEB_SERVICES_WEB_ROUTER_H

#include "LLServices/Networking/HttpRequestHandler.h"

#include <string>
#include <vector>

class WebRoutable;

class WebRouter final : public LLServices::HttpRequestHandler {
public:
    explicit WebRouter(WebRoutable *defaultRoutable);
    ~WebRouter();

    void handle(LLServices::HttpRequest &&request) override;

    void handleSubpath(std::string &&subpath, WebRoutable *routable);

private:
    struct SubpathRoutable {
        std::string subpath;
        WebRoutable *routable;
    };

    WebRoutable *m_defaultRoutable;
    std::vector<SubpathRoutable> m_subpaths;
};

#endif

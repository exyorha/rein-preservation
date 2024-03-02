#ifndef WEB_SERVICES_WEB_ROUTABLE_H
#define WEB_SERVICES_WEB_ROUTABLE_H

#include <string_view>

namespace LLServices {
    class HttpRequest;
}

class WebRoutable {
protected:
    WebRoutable();
    ~WebRoutable();

public:
    WebRoutable(const WebRoutable &other) = delete;
    WebRoutable &operator =(const WebRoutable &other) = delete;

    virtual void handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) = 0;
};

#endif

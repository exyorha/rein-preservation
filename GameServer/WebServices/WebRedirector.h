#ifndef WEB_REDIRECTOR_H
#define WEB_REDIRECTOR_H

#include "WebServices/WebRoutable.h"

class WebRedirector final : public WebRoutable {
public:
    WebRedirector();
    ~WebRedirector();

    WebRedirector(const WebRedirector &other) = delete;
    WebRedirector &operator =(const WebRedirector &other) = delete;

    void handle(const std::string_view &path, LLServices::HttpRequest &&request) override;

};

#endif


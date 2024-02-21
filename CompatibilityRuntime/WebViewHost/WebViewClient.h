#ifndef WEB_VIEW_CLIENT_H
#define WEB_VIEW_CLIENT_H

#include <include/cef_client.h>

class WebViewClient final : public CefClient {
private:
    ~WebViewClient();

    IMPLEMENT_REFCOUNTING(WebViewClient);

public:
    WebViewClient();
};

#endif

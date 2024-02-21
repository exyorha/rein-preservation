#ifndef WEB_VIEW_APP_H
#define WEB_VIEW_APP_H

#include <include/cef_app.h>

class WebViewApp final : public CefApp {
private:
    ~WebViewApp();

    IMPLEMENT_REFCOUNTING(WebViewApp);

public:
    WebViewApp();

};

#endif

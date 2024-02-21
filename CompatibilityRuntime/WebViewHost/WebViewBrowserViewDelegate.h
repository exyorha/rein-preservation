#ifndef WEB_VIEW_BROWSER_VIEW_DELEGATE_H
#define WEB_VIEW_BROWSER_VIEW_DELEGATE_H

#include <include/views/cef_browser_view_delegate.h>

class WebViewBrowserViewDelegate final : public CefBrowserViewDelegate {
private:
    ~WebViewBrowserViewDelegate();

    IMPLEMENT_REFCOUNTING(WebViewBrowserViewDelegate);

public:
    WebViewBrowserViewDelegate();
};

#endif

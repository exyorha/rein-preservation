#ifndef WEB_VIEW_CONTAINER_DELEGATE_H
#define WEB_VIEW_CONTAINER_DELEGATE_H

#include <include/views/cef_window_delegate.h>

#include <include/views/cef_window.h>

class WebViewContainerDelegate final : public CefWindowDelegate {
private:
    ~WebViewContainerDelegate();

    IMPLEMENT_REFCOUNTING(WebViewContainerDelegate);

public:
    WebViewContainerDelegate();
};

#endif

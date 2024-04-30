#ifndef WEB_VIEW_CLIENT_H
#define WEB_VIEW_CLIENT_H

#include <include/cef_client.h>

class WebViewRenderHandler;

class WebViewClient final : public CefClient {
private:
    ~WebViewClient();

    IMPLEMENT_REFCOUNTING(WebViewClient);

public:
    explicit WebViewClient(const CefRefPtr<WebViewRenderHandler> &renderHandler);

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;

private:
    CefRefPtr<WebViewRenderHandler> m_renderHandler;
};

#endif

#include "WebViewClient.h"
#include "WebViewRenderHandler.h"

WebViewClient::WebViewClient(const CefRefPtr<WebViewRenderHandler> &renderHandler) : m_renderHandler(renderHandler) {

}

WebViewClient::~WebViewClient() = default;

CefRefPtr<CefRenderHandler> WebViewClient::GetRenderHandler() {
    return m_renderHandler;
}

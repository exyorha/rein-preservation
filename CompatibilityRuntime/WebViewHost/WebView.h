#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <cstdint>
#include <memory>

#include <include/cef_browser.h>

class WebViewRenderHandler;
class WebViewSharedImageBuffer;

class WebView {
public:
    WebView(int x, int y, int width, int height, intptr_t parentWindow,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    ~WebView();

    WebView(const WebView &other) = delete;
    WebView &operator =(const WebView &other) = delete;


    void addJavaScript(const std::string & jsString, const std::string & identifier);
    void addPermissionTrustDomain(const std::string & domain);
    void addSslExceptionDomain(const std::string & domain);
    void addUrlScheme(const std::string & scheme);
    bool animateTo(int32_t x, int32_t y, int32_t width, int32_t height, float duration, float delay,
                           const std::string & identifier);
    bool canGoBack();
    bool canGoForward();
    void cleanCache();
    void evaluateJavaScript(const std::string & jsString, const std::string & identifier);
    std::string getUrl();
    std::string getUserAgent();
    float getWebViewAlpha();
    void goBack();
    void goForward();
    bool hide(bool fade, int32_t edge, float duration, const std::string & identifier);
    void load(const std::string &url);
    void loadHTMLString(const std::string &html, const std::string &baseUrl);
    void print();
    void reload();
    void removePermissionTrustDomain(const std::string & domain);
    void removeSslExceptionDomain(const std::string & domain);
    void removeUrlScheme(const std::string & scheme);
    void scrollTo(int32_t x, int32_t y, bool animated);
    void setAllowHTTPAuthPopUpWindow(bool arg2);
    void setBackButtonEnabled(bool enabled);
    void setBackgroundColor(float r, float g, float b, float a);
    void setBouncesEnabled(bool flag);
    void setCalloutEnabled(bool enabled);
    void setDefaultFontSize(int32_t enabled);
    void setFrame(int32_t x, int32_t y, int32_t width, int32_t height,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    void setHeaderField(const std::string & key, const std::string & value);
    void setHorizontalScrollBarEnabled(bool enabled);
    void setImmersiveModeEnabled(bool enabled);
    void setLoadWithOverviewMode(bool overview);
    void setOpenLinksInExternalBrowser(bool flag);
    void setPosition(int32_t x, int32_t y);
    void setShowSpinnerWhileLoading(bool show);
    void setSize(int32_t width, int32_t height,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    void setSpinnerText(const std::string & text);
    void setSupportMultipleWindows(bool flag);
    void setUseWideViewPort(bool use);
    void setUserAgent(const std::string & userAgent);
    void setUserInteractionEnabled(bool enalbed);
    void setVerticalScrollBarEnabled(bool flag);
    void setWebViewAlpha(float alpha);
    void setZoomEnabled(bool flag);
    bool show(bool fade, int32_t edge, float duration, const std::string & identifier);
    void showWebViewDialog(bool flag);
    void stop();

    void dispatchTouchEvent(const cef_touch_event_t &event);

private:
    CefRefPtr<WebViewRenderHandler> m_renderHandler;
    CefRefPtr<CefBrowser> m_browser;
};

#endif

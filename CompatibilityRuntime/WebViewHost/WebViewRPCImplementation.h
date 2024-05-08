#ifndef WEB_VIEW_RPC_IMPLEMENTATION_H
#define WEB_VIEW_RPC_IMPLEMENTATION_H

#include <cstdint>
#include <string>
#include <memory>
#include <unordered_map>

#include <include/internal/cef_types_wrappers.h>

class WebView;
class WebViewSharedImageBuffer;

namespace webview::protocol {
    class RPCMessage;
}

class WebViewRPCImplementation {
public:
    WebViewRPCImplementation();
    ~WebViewRPCImplementation();

    WebViewRPCImplementation(const WebViewRPCImplementation &other) = delete;
    WebViewRPCImplementation &operator =(const WebViewRPCImplementation &other) = delete;

    void setLogLevel(int32_t level);

    void setAllowAutoPlay(bool flag);
    void setAllowJavaScriptOpenWindow(bool flag);
    void setJavaScriptEnabled(bool flag);
    void setWebContentsDebuggingEnabled(bool enabled);

    void clearCookies();
    std::string getCookie(const std::string & arg1, const std::string & arg2);
    void setCookie(const std::string & url, const std::string & cookie);

    void clearHttpAuthUsernamePassword(const std::string & host, const std::string & realm);

    void init(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, intptr_t parentWindow,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    void destroy(const std::string & name);

    void addJavaScript(const std::string & arg1, const std::string & jsString, const std::string & identifier);
    void addPermissionTrustDomain(const std::string & name, const std::string & domain);
    void addSslExceptionDomain(const std::string & name, const std::string & domain);
    void addUrlScheme(const std::string & name, const std::string & scheme);
    bool animateTo(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, float duration, float delay,
                           const std::string & identifier);
    bool canGoBack(const std::string & name);
    bool canGoForward(const std::string & name);
    void cleanCache(const std::string & name);
    void evaluateJavaScript(const std::string & name, const std::string & jsString, const std::string & identifier);
    std::string getUrl(const std::string & name);
    std::string getUserAgent(const std::string & name);
    float getWebViewAlpha(const std::string & name);
    void goBack(const std::string & name);
    void goForward(const std::string & name);
    bool hide(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    void load(const std::string &name, const std::string &url);
    void loadHTMLString(const std::string &name, const std::string &html, const std::string &baseUrl);
    void print(const std::string & name);
    void reload(const std::string &name);
    void removePermissionTrustDomain(const std::string & name, const std::string & domain);
    void removeSslExceptionDomain(const std::string & name, const std::string & domain);
    void removeUrlScheme(const std::string & name, const std::string & scheme);
    void scrollTo(const std::string & name, int32_t x, int32_t y, bool animated);
    void setAllowHTTPAuthPopUpWindow(const std::string & name, bool arg2);
    void setBackButtonEnabled(const std::string & name, bool enabled);
    void setBackgroundColor(const std::string & name, float r, float g, float b, float a);
    void setBouncesEnabled(const std::string & name, bool flag);
    void setCalloutEnabled(const std::string & name, bool enabled);
    void setDefaultFontSize(const std::string & name, int32_t enabled);
    void setFrame(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    void setHeaderField(const std::string & name, const std::string & key, const std::string & value);
    void setHorizontalScrollBarEnabled(const std::string & name, bool enabled);
    void setImmersiveModeEnabled(const std::string & name, bool enabled);
    void setLoadWithOverviewMode(const std::string & name, bool overview);
    void setOpenLinksInExternalBrowser(const std::string & name, bool flag);
    void setPosition(const std::string & name, int32_t x, int32_t y);
    void setShowSpinnerWhileLoading(const std::string & name, bool show);
    void setSize(const std::string & name, int32_t width, int32_t height,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory);
    void setSpinnerText(const std::string & name, const std::string & text);
    void setSupportMultipleWindows(const std::string & name, bool flag);
    void setUseWideViewPort(const std::string & name, bool use);
    void setUserAgent(const std::string & name, const std::string & userAgent);
    void setUserInteractionEnabled(const std::string & name, bool enalbed);
    void setVerticalScrollBarEnabled(const std::string & name, bool flag);
    void setWebViewAlpha(const std::string & name, float alpha);
    void setZoomEnabled(const std::string & name, bool flag);
    bool show(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    void showWebViewDialog(const std::string & name, bool flag);
    void stop(const std::string & name);

    void handleNonCallMessage(std::unique_ptr<webview::protocol::RPCMessage> &&request);

private:
    WebView &get(const std::string &name);

    CefBrowserSettings m_settings;
    std::unordered_map<std::string, std::unique_ptr<WebView>> m_webViews;

};

#endif

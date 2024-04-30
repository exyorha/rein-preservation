#ifndef WEB_VIEW_CEF_WEB_VIEW_IMPLEMENTATION_H
#define WEB_VIEW_CEF_WEB_VIEW_IMPLEMENTATION_H

#include <WebView/WebViewImplementation.h>
#include <WebViewHostClient.h>

#include <unordered_map>
#include <memory>
#include <mutex>

class CEFSurface;

class CEFWebViewImplementation final : public WebViewImplementation {
public:
    explicit CEFWebViewImplementation(const WebViewHostClientConfiguration &configuration);
    ~CEFWebViewImplementation() override;

    void setLogLevel(int32_t level) override;

    float screenHeight() override;
    float screenWidth() override;

    void setAllowAutoPlay(bool flag) override;
    void setAllowJavaScriptOpenWindow(bool flag) override;
    void setJavaScriptEnabled(bool flag) override;
    void setWebContentsDebuggingEnabled(bool enabled) override;

    void clearCookies() override;
    std::string getCookie(const std::string & arg1, const std::string & arg2) override;
    void setCookie(const std::string & url, const std::string & cookie) override;

    void clearHttpAuthUsernamePassword(const std::string & host, const std::string & realm) override;

    void init(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height) override;
    void destroy(const std::string & name) override;

    void addJavaScript(const std::string & arg1, const std::string & jsString, const std::string & identifier) override;
    void addPermissionTrustDomain(const std::string & name, const std::string & domain) override;
    void addSslExceptionDomain(const std::string & name, const std::string & domain) override;
    void addUrlScheme(const std::string & name, const std::string & scheme) override;
    bool animateTo(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, float duration, float delay,
                           const std::string & identifier) override;
    bool canGoBack(const std::string & name) override;
    bool canGoForward(const std::string & name) override;
    void cleanCache(const std::string & name) override;
    void evaluateJavaScript(const std::string & name, const std::string & jsString, const std::string & identifier) override;
    std::string getUrl(const std::string & name) override;
    std::string getUserAgent(const std::string & name) override;
    float getWebViewAlpha(const std::string & name) override;
    void goBack(const std::string & name) override;
    void goForward(const std::string & name) override;
    bool hide(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier) override;
    void load(const std::string &name, const std::string &url) override;
    void loadHTMLString(const std::string &name, const std::string &html, const std::string &baseUrl) override;
    void print(const std::string & name) override;
    void reload(const std::string &name) override;
    void removePermissionTrustDomain(const std::string & name, const std::string & domain) override;
    void removeSslExceptionDomain(const std::string & name, const std::string & domain) override;
    void removeUrlScheme(const std::string & name, const std::string & scheme) override;
    void scrollTo(const std::string & name, int32_t x, int32_t y, bool animated) override;
    void setAllowHTTPAuthPopUpWindow(const std::string & name, bool arg2) override;
    void setBackButtonEnabled(const std::string & name, bool enabled) override;
    void setBackgroundColor(const std::string & name, float r, float g, float b, float a) override;
    void setBouncesEnabled(const std::string & name, bool flag) override;
    void setCalloutEnabled(const std::string & name, bool enabled) override;
    void setDefaultFontSize(const std::string & name, int32_t enabled) override;
    void setFrame(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height) override;
    void setHeaderField(const std::string & name, const std::string & key, const std::string & value) override;
    void setHorizontalScrollBarEnabled(const std::string & name, bool enabled) override;
    void setImmersiveModeEnabled(const std::string & name, bool enabled) override;
    void setLoadWithOverviewMode(const std::string & name, bool overview) override;
    void setOpenLinksInExternalBrowser(const std::string & name, bool flag) override;
    void setPosition(const std::string & name, int32_t x, int32_t y) override;
    void setShowSpinnerWhileLoading(const std::string & name, bool show) override;
    void setSize(const std::string & name, int32_t width, int32_t height) override;
    void setSpinnerText(const std::string & name, const std::string & text) override;
    void setSupportMultipleWindows(const std::string & name, bool flag) override;
    void setUseWideViewPort(const std::string & name, bool use) override;
    void setUserAgent(const std::string & name, const std::string & userAgent) override;
    void setUserInteractionEnabled(const std::string & name, bool enalbed) override;
    void setVerticalScrollBarEnabled(const std::string & name, bool flag) override;
    void setWebViewAlpha(const std::string & name, float alpha) override;
    void setZoomEnabled(const std::string & name, bool flag) override;
    bool show(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier) override;
    void showWebViewDialog(const std::string & name, bool flag) override;
    void stop(const std::string & name) override;

private:
    int64_t getParentWindowHandle();
    static std::string getWebViewHostPath();

    CEFSurface *getSurfaceLocked(const std::string &name) const;

    WebViewHostClient m_client;
    std::mutex m_surfacesMutex;
    std::unordered_map<std::string, std::unique_ptr<CEFSurface>> m_surfaces;
};

#endif

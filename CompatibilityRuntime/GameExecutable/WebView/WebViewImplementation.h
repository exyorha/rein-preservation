#ifndef WEB_VIEW_WEB_VIEW_IMPLEMENTATION_H
#define WEB_VIEW_WEB_VIEW_IMPLEMENTATION_H

#include <cstdint>
#include <string>

class WebViewImplementation {
public:
    WebViewImplementation();
    virtual ~WebViewImplementation();

    WebViewImplementation(const WebViewImplementation &other) = delete;
    WebViewImplementation &operator =(const WebViewImplementation &other) = delete;

    virtual void setLogLevel(int32_t level);

    virtual float screenHeight();
    virtual float screenWidth();

    virtual void setAllowAutoPlay(bool flag);
    virtual void setAllowJavaScriptOpenWindow(bool flag);
    virtual void setJavaScriptEnabled(bool flag);
    virtual void setWebContentsDebuggingEnabled(bool enabled);

    virtual void clearCookies();
    virtual std::string getCookie(const std::string & arg1, const std::string & arg2);
    virtual void setCookie(const std::string & url, const std::string & cookie);

    virtual void clearHttpAuthUsernamePassword(const std::string & host, const std::string & realm);

    virtual void init(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height);
    virtual void destroy(const std::string & name);

    virtual void addJavaScript(const std::string & arg1, const std::string & jsString, const std::string & identifier);
    virtual void addPermissionTrustDomain(const std::string & name, const std::string & domain);
    virtual void addSslExceptionDomain(const std::string & name, const std::string & domain);
    virtual void addUrlScheme(const std::string & name, const std::string & scheme);
    virtual bool animateTo(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, float duration, float delay,
                           const std::string & identifier);
    virtual bool canGoBack(const std::string & name);
    virtual bool canGoForward(const std::string & name);
    virtual void cleanCache(const std::string & name);
    virtual void evaluateJavaScript(const std::string & name, const std::string & jsString, const std::string & identifier);
    virtual std::string getUrl(const std::string & name);
    virtual std::string getUserAgent(const std::string & name);
    virtual float getWebViewAlpha(const std::string & name);
    virtual void goBack(const std::string & name);
    virtual void goForward(const std::string & name);
    virtual bool hide(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    virtual void load(const std::string &name, const std::string &url);
    virtual void loadHTMLString(const std::string &name, const std::string &html, const std::string &baseUrl);
    virtual void print(const std::string & name);
    virtual void reload(const std::string &name);
    virtual void removePermissionTrustDomain(const std::string & name, const std::string & domain);
    virtual void removeSslExceptionDomain(const std::string & name, const std::string & domain);
    virtual void removeUrlScheme(const std::string & name, const std::string & scheme);
    virtual void scrollTo(const std::string & name, int32_t x, int32_t y, bool animated);
    virtual void setAllowHTTPAuthPopUpWindow(const std::string & name, bool arg2);
    virtual void setBackButtonEnabled(const std::string & name, bool enabled);
    virtual void setBackgroundColor(const std::string & name, float r, float g, float b, float a);
    virtual void setBouncesEnabled(const std::string & name, bool flag);
    virtual void setCalloutEnabled(const std::string & name, bool enabled);
    virtual void setDefaultFontSize(const std::string & name, int32_t enabled);
    virtual void setFrame(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height);
    virtual void setHeaderField(const std::string & name, const std::string & key, const std::string & value);
    virtual void setHorizontalScrollBarEnabled(const std::string & name, bool enabled);
    virtual void setImmersiveModeEnabled(const std::string & name, bool enabled);
    virtual void setLoadWithOverviewMode(const std::string & name, bool overview);
    virtual void setOpenLinksInExternalBrowser(const std::string & name, bool flag);
    virtual void setPosition(const std::string & name, int32_t x, int32_t y);
    virtual void setShowSpinnerWhileLoading(const std::string & name, bool show);
    virtual void setSize(const std::string & name, int32_t width, int32_t height);
    virtual void setSpinnerText(const std::string & name, const std::string & text);
    virtual void setSupportMultipleWindows(const std::string & name, bool flag);
    virtual void setUseWideViewPort(const std::string & name, bool use);
    virtual void setUserAgent(const std::string & name, const std::string & userAgent);
    virtual void setUserInteractionEnabled(const std::string & name, bool enalbed);
    virtual void setVerticalScrollBarEnabled(const std::string & name, bool flag);
    virtual void setWebViewAlpha(const std::string & name, float alpha);
    virtual void setZoomEnabled(const std::string & name, bool flag);
    virtual bool show(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    virtual void showWebViewDialog(const std::string & name, bool flag);
    virtual void stop(const std::string & name);
};

#endif

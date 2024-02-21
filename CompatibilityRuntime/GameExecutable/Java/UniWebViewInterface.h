#ifndef JAVA_UNI_WEB_VIEW_INTERFACE_H
#define JAVA_UNI_WEB_VIEW_INTERFACE_H

#include <Java/JNIClass.h>

class JNIString;

class WebViewImplementation;

class UniWebViewInterface final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

    static void prepare();
    static void setLogLevel(int32_t level);

    static float screenHeight();
    static float screenWidth();

    static void setAllowAutoPlay(bool flag);
    static void setAllowJavaScriptOpenWindow(bool flag);
    static void setJavaScriptEnabled(bool flag);
    static void setWebContentsDebuggingEnabled(bool enabled);

    static void clearCookies();
    static std::shared_ptr<JNIObject> getCookie(const std::string & arg1, const std::string & arg2);
    static void setCookie(const std::string & url, const std::string & cookie);

    static void clearHttpAuthUsernamePassword(const std::string & host, const std::string & realm);

    static void init(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height);
    static void destroy(const std::string & name);

    static void addJavaScript(const std::string & arg1, const std::string & jsString, const std::string & identifier);
    static void addPermissionTrustDomain(const std::string & name, const std::string & domain);
    static void addSslExceptionDomain(const std::string & name, const std::string & domain);
    static void addUrlScheme(const std::string & name, const std::string & scheme);
    static bool animateTo(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, float duration, float delay, const std::string & identifier);
    static bool canGoBack(const std::string & name);
    static bool canGoForward(const std::string & name);
    static void cleanCache(const std::string & name);
    static void evaluateJavaScript(const std::string & name, const std::string & jsString, const std::string & identifier);
    static std::shared_ptr<JNIObject> getUrl(const std::string & name);
    static std::shared_ptr<JNIObject> getUserAgent(const std::string & name);
    static float getWebViewAlpha(const std::string & name);
    static void goBack(const std::string & name);
    static void goForward(const std::string & name);
    static bool hide(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    static void load(const std::string &name, const std::string &url);
    static void loadHTMLString(const std::string &name, const std::string &html, const std::string &baseUrl);
    static void print(const std::string & name);
    static void reload(const std::string &name);
    static void removePermissionTrustDomain(const std::string & name, const std::string & domain);
    static void removeSslExceptionDomain(const std::string & name, const std::string & domain);
    static void removeUrlScheme(const std::string & name, const std::string & scheme);
    static void scrollTo(const std::string & name, int32_t x, int32_t y, bool animated);
    static void setAllowHTTPAuthPopUpWindow(const std::string & name, bool arg2);
    static void setBackButtonEnabled(const std::string & name, bool enabled);
    static void setBackgroundColor(const std::string & name, float r, float g, float b, float a);
    static void setBouncesEnabled(const std::string & name, bool flag);
    static void setCalloutEnabled(const std::string & name, bool enabled);
    static void setDefaultFontSize(const std::string & name, int32_t enabled);
    static void setFrame(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height);
    static void setHeaderField(const std::string & name, const std::string & key, const std::string & value);
    static void setHorizontalScrollBarEnabled(const std::string & name, bool enabled);
    static void setImmersiveModeEnabled(const std::string & name, bool enabled);
    static void setLoadWithOverviewMode(const std::string & name, bool overview);
    static void setOpenLinksInExternalBrowser(const std::string & name, bool flag);
    static void setPosition(const std::string & name, int32_t x, int32_t y);
    static void setShowSpinnerWhileLoading(const std::string & name, bool show);
    static void setSize(const std::string & name, int32_t width, int32_t height);
    static void setSpinnerText(const std::string & name, const std::string & text);
    static void setSupportMultipleWindows(const std::string & name, bool flag);
    static void setUseWideViewPort(const std::string & name, bool use);
    static void setUserAgent(const std::string & name, const std::string & userAgent);
    static void setUserInteractionEnabled(const std::string & name, bool enalbed);
    static void setVerticalScrollBarEnabled(const std::string & name, bool flag);
    static void setWebViewAlpha(const std::string & name, float alpha);
    static void setZoomEnabled(const std::string & name, bool flag);
    static bool show(const std::string & name, bool fade, int32_t edge, float duration, const std::string & identifier);
    static void showWebViewDialog(const std::string & name, bool flag);
    static void stop(const std::string & name);

private:
    static std::unique_ptr<WebViewImplementation> m_impl;
};

#endif



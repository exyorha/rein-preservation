#ifndef JAVA_UNI_WEB_VIEW_INTERFACE_H
#define JAVA_UNI_WEB_VIEW_INTERFACE_H

#include <Java/JNIClass.h>

class JNIString;

class UniWebViewInterface final : public JNIObject {
public:

    static std::shared_ptr<JNIClass> makeClass();

    static void addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3);
    static void addPermissionTrustDomain(const std::string & arg1, const std::string & arg2);
    static void addSslExceptionDomain(const std::string & arg1, const std::string & arg2);
    static void addUrlScheme(const std::string & arg1, const std::string & arg2);
    static bool animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7, const std::string & arg8);
    static bool canGoBack(const std::string & arg1);
    static bool canGoForward(const std::string & arg1);
    static void cleanCache(const std::string & arg1);
    static void clearCookies();
    static void clearHttpAuthUsernamePassword(const std::string & arg1, const std::string & arg2);
    static void destroy(const std::string & arg1);
    static void evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3);
    static std::shared_ptr<JNIObject> getCookie(const std::string & arg1, const std::string & arg2);
    static std::shared_ptr<JNIObject> getUrl(const std::string & arg1);
    static std::shared_ptr<JNIObject> getUserAgent(const std::string & arg1);
    static float getWebViewAlpha(const std::string & arg1);
    static void goBack(const std::string & arg1);
    static void goForward(const std::string & arg1);
    static bool hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5);
    static void init(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5);
    static void load(const std::string & arg1, const std::string & arg2);
    static void loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3);
    static void prepare();
    static void print(const std::string & arg1);
    static void reload(const std::string & arg1);
    static void removePermissionTrustDomain(const std::string & arg1, const std::string & arg2);
    static void removeSslExceptionDomain(const std::string & arg1, const std::string & arg2);
    static void removeUrlScheme(const std::string & arg1, const std::string & arg2);
    static float screenHeight();
    static float screenWidth();
    static void scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4);
    static void setAllowAutoPlay(bool arg1);
    static void setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2);
    static void setAllowJavaScriptOpenWindow(bool arg1);
    static void setBackButtonEnabled(const std::string & arg1, bool arg2);
    static void setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5);
    static void setBouncesEnabled(const std::string & arg1, bool arg2);
    static void setCalloutEnabled(const std::string & arg1, bool arg2);
    static void setCookie(const std::string & arg1, const std::string & arg2);
    static void setDefaultFontSize(const std::string & arg1, int32_t arg2);
    static void setFrame(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5);
    static void setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3);
    static void setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2);
    static void setImmersiveModeEnabled(const std::string & arg1, bool arg2);
    static void setJavaScriptEnabled(bool arg1);
    static void setLoadWithOverviewMode(const std::string & arg1, bool arg2);
    static void setLogLevel(int32_t arg1);
    static void setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2);
    static void setPosition(const std::string & arg1, int32_t arg2, int32_t arg3);
    static void setShowSpinnerWhileLoading(const std::string & arg1, bool arg2);
    static void setSize(const std::string & arg1, int32_t arg2, int32_t arg3);
    static void setSpinnerText(const std::string & arg1, const std::string & arg2);
    static void setSupportMultipleWindows(const std::string & arg1, bool arg2);
    static void setUseWideViewPort(const std::string & arg1, bool arg2);
    static void setUserAgent(const std::string & arg1, const std::string & arg2);
    static void setUserInteractionEnabled(const std::string & arg1, bool arg2);
    static void setVerticalScrollBarEnabled(const std::string & arg1, bool arg2);
    static void setWebContentsDebuggingEnabled(bool arg1);
    static void setWebViewAlpha(const std::string & arg1, float arg2);
    static void setZoomEnabled(const std::string & arg1, bool arg2);
    static bool show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5);
    static void showWebViewDialog(const std::string & arg1, bool arg2);
    static void stop(const std::string & arg1);

};

#endif



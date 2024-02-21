#include <WebView/WebViewImplementation.h>

WebViewImplementation::WebViewImplementation() = default;

WebViewImplementation::~WebViewImplementation() = default;

void WebViewImplementation::addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("WebViewImplementation::addJavaScript(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void WebViewImplementation::addPermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::addPermissionTrustDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::addSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::addSslExceptionDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::addUrlScheme(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::addUrlScheme(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

bool WebViewImplementation::animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7,
                                      const std::string & arg8) {
    printf("WebViewImplementation::animateTo(%s, %d, %d, %d, %d, %f, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5, arg6, arg7, arg8.c_str());
    return false;
}

bool WebViewImplementation::canGoBack(const std::string & arg1) {
    printf("WebViewImplementation::canGoBack(%s) stub!\n", arg1.c_str());
    return false;
}

bool WebViewImplementation::canGoForward(const std::string & arg1) {
    printf("WebViewImplementation::canGoForward(%s) stub!\n", arg1.c_str());
    return false;
}

void WebViewImplementation::cleanCache(const std::string & arg1) {
    printf("WebViewImplementation::cleanCache(%s) stub!\n", arg1.c_str());
}

void WebViewImplementation::clearCookies() {
    printf("WebViewImplementation::clearCookies() stub!\n");
}

void WebViewImplementation::clearHttpAuthUsernamePassword(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::clearHttpAuthUsernamePassword(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::destroy(const std::string & arg1) {
    printf("WebViewImplementation::destroy(%s) stub!\n", arg1.c_str());
}

void WebViewImplementation::evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("WebViewImplementation::evaluateJavaScript(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

std::string WebViewImplementation::getCookie(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::getCookie(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
    return "";
}

std::string WebViewImplementation::getUrl(const std::string & arg1) {
    printf("WebViewImplementation::getUrl(%s) stub!\n", arg1.c_str());
    return "";
}

std::string WebViewImplementation::getUserAgent(const std::string & arg1) {
    printf("WebViewImplementation::getUserAgent(%s) stub!\n", arg1.c_str());
    return "";
}

float WebViewImplementation::getWebViewAlpha(const std::string & arg1) {
    printf("WebViewImplementation::getWebViewAlpha(%s) stub!\n", arg1.c_str());
    return 1.0f;
}

void WebViewImplementation::goBack(const std::string & arg1) {
    printf("WebViewImplementation::goBack(%s) stub!\n", arg1.c_str());
}

void WebViewImplementation::goForward(const std::string & arg1) {
    printf("WebViewImplementation::goForward(%s) stub!\n", arg1.c_str());
}

bool WebViewImplementation::hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    printf("WebViewImplementation::hide(%s, %d, %d, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5.c_str());
    return false;
}

void WebViewImplementation::init(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    printf("WebViewImplementation::init(%s, %d, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void WebViewImplementation::load(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::load(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("WebViewImplementation::loadHTMLString(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void WebViewImplementation::print(const std::string & arg1) {
    printf("WebViewImplementation::print(%s) stub!\n", arg1.c_str());
}

void WebViewImplementation::reload(const std::string & arg1) {
    printf("WebViewImplementation::reload(%s) stub!\n", arg1.c_str());
}

void WebViewImplementation::removePermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::removePermissionTrustDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::removeSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::removeSslExceptionDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::removeUrlScheme(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::removeUrlScheme(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

float WebViewImplementation::screenHeight() {
    printf("WebViewImplementation::screenHeight() stub!\n");
    return 640.0f;
}

float WebViewImplementation::screenWidth() {
    printf("WebViewImplementation::screenWidth() stub!\n");
    return 480.0f;
}

void WebViewImplementation::scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4) {
    printf("WebViewImplementation::scrollTo(%s, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4);
}

void WebViewImplementation::setAllowAutoPlay(bool arg1) {
    printf("WebViewImplementation::setAllowAutoPlay(%d) stub!\n", arg1);
}

void WebViewImplementation::setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setAllowHTTPAuthPopUpWindow(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setAllowJavaScriptOpenWindow(bool arg1) {
    printf("WebViewImplementation::setAllowJavaScriptOpenWindow(%d) stub!\n", arg1);
}

void WebViewImplementation::setBackButtonEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setBackButtonEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5) {
    printf("WebViewImplementation::setBackgroundColor(%s, %f, %f, %f, %f) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void WebViewImplementation::setBouncesEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setBouncesEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setCalloutEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setCalloutEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setCookie(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::setCookie(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::setDefaultFontSize(const std::string & arg1, int32_t arg2) {
    printf("WebViewImplementation::setDefaultFontSize(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setFrame(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    printf("WebViewImplementation::setFrame(%s, %d, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void WebViewImplementation::setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("WebViewImplementation::setHeaderField(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void WebViewImplementation::setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setHorizontalScrollBarEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setImmersiveModeEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setImmersiveModeEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setJavaScriptEnabled(bool arg1) {
    printf("WebViewImplementation::setJavaScriptEnabled(%d) stub!\n", arg1);
}

void WebViewImplementation::setLoadWithOverviewMode(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setLoadWithOverviewMode(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setLogLevel(int32_t arg1) {
    printf("WebViewImplementation::setLogLevel(%d) stub!\n", arg1);
}

void WebViewImplementation::setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setOpenLinksInExternalBrowser(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setPosition(const std::string & arg1, int32_t arg2, int32_t arg3) {
    printf("WebViewImplementation::setPosition(%s, %d, %d) stub!\n", arg1.c_str(), arg2, arg3);
}

void WebViewImplementation::setShowSpinnerWhileLoading(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setShowSpinnerWhileLoading(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setSize(const std::string & arg1, int32_t arg2, int32_t arg3) {
    printf("WebViewImplementation::setSize(%s, %d, %d) stub!\n", arg1.c_str(), arg2, arg3);
}

void WebViewImplementation::setSpinnerText(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::setSpinnerText(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::setSupportMultipleWindows(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setSupportMultipleWindows(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setUseWideViewPort(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setUseWideViewPort(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setUserAgent(const std::string & arg1, const std::string & arg2) {
    printf("WebViewImplementation::setUserAgent(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebViewImplementation::setUserInteractionEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setUserInteractionEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setVerticalScrollBarEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setVerticalScrollBarEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setWebContentsDebuggingEnabled(bool arg1) {
    printf("WebViewImplementation::setWebContentsDebuggingEnabled(%d) stub!\n", arg1);
}

void WebViewImplementation::setWebViewAlpha(const std::string & arg1, float arg2) {
    printf("WebViewImplementation::setWebViewAlpha(%s, %f) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::setZoomEnabled(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::setZoomEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

bool WebViewImplementation::show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    printf("WebViewImplementation::show(%s, %d, %d, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5.c_str());
    return false;
}

void WebViewImplementation::showWebViewDialog(const std::string & arg1, bool arg2) {
    printf("WebViewImplementation::showWebViewDialog(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void WebViewImplementation::stop(const std::string & arg1) {
    printf("WebViewImplementation::stop(%s) stub!\n", arg1.c_str());
}




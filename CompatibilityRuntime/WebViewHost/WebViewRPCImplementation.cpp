#include "WebViewRPCImplementation.h"
#include "include/internal/cef_types.h"
#include "include/cef_cookie.h"

#include "WebView.h"

WebViewRPCImplementation::WebViewRPCImplementation() = default;

WebViewRPCImplementation::~WebViewRPCImplementation() = default;

void WebViewRPCImplementation::setLogLevel(int32_t level) {
    printf("WebViewRPCImplementation::setLogLevel(%d) stub!\n", level);
}

void WebViewRPCImplementation::setAllowJavaScriptOpenWindow(bool arg1) {
    printf("WebViewRPCImplementation::setAllowJavaScriptOpenWindow(%d) stub!\n", arg1);
}

void WebViewRPCImplementation::setWebContentsDebuggingEnabled(bool enabled) {
    printf("WebViewRPCImplementation::setWebContentsDebuggingEnabled(%d) stub!\n", enabled);
}

void WebViewRPCImplementation::setJavaScriptEnabled(bool flag) {
    if(flag)
        m_settings.javascript = STATE_ENABLED;
    else
        m_settings.javascript = STATE_DISABLED;
}

void WebViewRPCImplementation::init(const std::string & name, int32_t x, int32_t y, int32_t width, int32_t height, intptr_t parentWindow, std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory) {

    auto webView = std::make_unique<WebView>(x, y, width, height, parentWindow, std::move(sharedMemory));
    if(!m_webViews.emplace(name, std::move(webView)).second)
        throw std::runtime_error("a WebView with this name already exists");
}

void WebViewRPCImplementation::destroy(const std::string & name) {
    m_webViews.erase(name);
}

void WebViewRPCImplementation::clearCookies() {
    CefCookieManager::GetGlobalManager(nullptr)->FlushStore(nullptr);
}

std::string WebViewRPCImplementation::getCookie(const std::string & arg1, const std::string & arg2) {
    printf("WebViewRPCImplementation::getCookie(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
    return "";
}

void WebViewRPCImplementation::setCookie(const std::string & url, const std::string & cookie) {
    printf("WebViewRPCImplementation::setCookie(%s, %s) stub!\n", url.c_str(), cookie.c_str());
}

void WebViewRPCImplementation::clearHttpAuthUsernamePassword(const std::string & host, const std::string & realm) {
    printf("WebViewRPCImplementation::clearHttpAuthUsernamePassword(%s, %s) stub!\n", host.c_str(), realm.c_str());
}

void WebViewRPCImplementation::setAllowAutoPlay(bool flag) {
    printf("WebViewRPCImplementation::setAllowAutoPlay(%d) stub!\n", flag);
}

WebView &WebViewRPCImplementation::get(const std::string &name) {
    auto it = m_webViews.find(name);
    if(it == m_webViews.end())
        throw std::runtime_error("no such web view exists");

    return *it->second;
}

void WebViewRPCImplementation::addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return get(arg1).addJavaScript(arg2, arg3);
}
void WebViewRPCImplementation::addPermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    return get(arg1).addPermissionTrustDomain(arg2);
}
void WebViewRPCImplementation::addSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    return get(arg1).addSslExceptionDomain(arg2);
}
void WebViewRPCImplementation::addUrlScheme(const std::string & arg1, const std::string & arg2) {
    return get(arg1).addUrlScheme(arg2);
}
bool WebViewRPCImplementation::animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7, const std::string & arg8) {
    return get(arg1).animateTo(arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}
bool WebViewRPCImplementation::canGoBack(const std::string & arg1) {
    return get(arg1).canGoBack();
}
bool WebViewRPCImplementation::canGoForward(const std::string & arg1) {
    return get(arg1).canGoForward();
}
void WebViewRPCImplementation::cleanCache(const std::string & arg1) {
    return get(arg1).cleanCache();
}
void WebViewRPCImplementation::evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return get(arg1).evaluateJavaScript(arg2, arg3);
}
std::string WebViewRPCImplementation::getUrl(const std::string & arg1) {
    return get(arg1).getUrl();
}
std::string WebViewRPCImplementation::getUserAgent(const std::string & arg1) {
    return get(arg1).getUserAgent();
}
float WebViewRPCImplementation::getWebViewAlpha(const std::string & arg1) {
    return get(arg1).getWebViewAlpha();
}
void WebViewRPCImplementation::goBack(const std::string & arg1) {
    return get(arg1).goBack();
}
void WebViewRPCImplementation::goForward(const std::string & arg1) {
    return get(arg1).goForward();
}
bool WebViewRPCImplementation::hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    return get(arg1).hide(arg2, arg3, arg4, arg5);
}
void WebViewRPCImplementation::load(const std::string & arg1, const std::string & arg2) {
    return get(arg1).load(arg2);
}
void WebViewRPCImplementation::loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return get(arg1).loadHTMLString(arg2, arg3);
}
void WebViewRPCImplementation::print(const std::string & arg1) {
    return get(arg1).print();
}
void WebViewRPCImplementation::reload(const std::string & arg1) {
    return get(arg1).reload();
}
void WebViewRPCImplementation::removePermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    return get(arg1).removePermissionTrustDomain(arg2);
}
void WebViewRPCImplementation::removeSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    return get(arg1).removeSslExceptionDomain(arg2);
}
void WebViewRPCImplementation::removeUrlScheme(const std::string & arg1, const std::string & arg2) {
    return get(arg1).removeUrlScheme(arg2);
}
void WebViewRPCImplementation::scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4) {
    return get(arg1).scrollTo(arg2, arg3, arg4);
}
void WebViewRPCImplementation::setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2) {
    return get(arg1).setAllowHTTPAuthPopUpWindow(arg2);
}
void WebViewRPCImplementation::setBackButtonEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setBackButtonEnabled(arg2);
}
void WebViewRPCImplementation::setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5) {
    return get(arg1).setBackgroundColor(arg2, arg3, arg4, arg5);
}
void WebViewRPCImplementation::setBouncesEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setBouncesEnabled(arg2);
}
void WebViewRPCImplementation::setCalloutEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setCalloutEnabled(arg2);
}
void WebViewRPCImplementation::setDefaultFontSize(const std::string & arg1, int32_t arg2) {
    return get(arg1).setDefaultFontSize(arg2);
}
void WebViewRPCImplementation::setFrame(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory) {
    return get(arg1).setFrame(arg2, arg3, arg4, arg5, std::move(sharedMemory));
}
void WebViewRPCImplementation::setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return get(arg1).setHeaderField(arg2, arg3);
}
void WebViewRPCImplementation::setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setHorizontalScrollBarEnabled(arg2);
}
void WebViewRPCImplementation::setImmersiveModeEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setImmersiveModeEnabled(arg2);
}
void WebViewRPCImplementation::setLoadWithOverviewMode(const std::string & arg1, bool arg2) {
    return get(arg1).setLoadWithOverviewMode(arg2);
}
void WebViewRPCImplementation::setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2) {
    return get(arg1).setOpenLinksInExternalBrowser(arg2);
}
void WebViewRPCImplementation::setPosition(const std::string & arg1, int32_t arg2, int32_t arg3) {
    return get(arg1).setPosition(arg2, arg3);
}
void WebViewRPCImplementation::setShowSpinnerWhileLoading(const std::string & arg1, bool arg2) {
    return get(arg1).setShowSpinnerWhileLoading(arg2);
}
void WebViewRPCImplementation::setSize(const std::string & arg1, int32_t arg2, int32_t arg3,
              std::unique_ptr<WebViewSharedImageBuffer> &&sharedMemory) {
    return get(arg1).setSize(arg2, arg3, std::move(sharedMemory));
}
void WebViewRPCImplementation::setSpinnerText(const std::string & arg1, const std::string & arg2) {
    return get(arg1).setSpinnerText(arg2);
}
void WebViewRPCImplementation::setSupportMultipleWindows(const std::string & arg1, bool arg2) {
    return get(arg1).setSupportMultipleWindows(arg2);
}
void WebViewRPCImplementation::setUseWideViewPort(const std::string & arg1, bool arg2) {
    return get(arg1).setUseWideViewPort(arg2);
}
void WebViewRPCImplementation::setUserAgent(const std::string & arg1, const std::string & arg2) {
    return get(arg1).setUserAgent(arg2);
}
void WebViewRPCImplementation::setUserInteractionEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setUserInteractionEnabled(arg2);
}
void WebViewRPCImplementation::setVerticalScrollBarEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setVerticalScrollBarEnabled(arg2);
}
void WebViewRPCImplementation::setWebViewAlpha(const std::string & arg1, float arg2) {
    return get(arg1).setWebViewAlpha(arg2);
}
void WebViewRPCImplementation::setZoomEnabled(const std::string & arg1, bool arg2) {
    return get(arg1).setZoomEnabled(arg2);
}
bool WebViewRPCImplementation::show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    return get(arg1).show(arg2, arg3, arg4, arg5);
}
void WebViewRPCImplementation::showWebViewDialog(const std::string & arg1, bool arg2) {
    return get(arg1).showWebViewDialog(arg2);
}
void WebViewRPCImplementation::stop(const std::string & arg1) {
    return get(arg1).stop();
}

#include "WebView.h"
#include "WebViewContainerDelegate.h"
#include "WebViewClient.h"
#include "WebViewBrowserViewDelegate.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <include/internal/cef_types_linux.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

WebView::WebView(int x, int y, int width, int height, intptr_t parentWindow) {
    m_window = CefWindow::CreateTopLevelWindow(CefRefPtr<WebViewContainerDelegate>(new WebViewContainerDelegate));

#ifdef _WIN32
    HWND cefWindow = m_window->GetWindowHandle();
    SetParent(cefWindow, reinterpret_cast<HWND>(parentWindow));
    SetWindowPos(cefWindow, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED);
#else
    auto display = cef_get_xdisplay();

    Window cefWindow = m_window->GetWindowHandle();
    XReparentWindow(display, cefWindow, static_cast<Window>(parentWindow), 0, 0);
    setFrame(x, y, width, height);
#endif

    CefBrowserSettings browserSettings;
    m_browser = CefBrowserView::CreateBrowserView(
        CefRefPtr<WebViewClient>(new WebViewClient),
        "",
        browserSettings,
        nullptr,
        nullptr,
        CefRefPtr<WebViewBrowserViewDelegate>(new WebViewBrowserViewDelegate));

    m_window->AddChildView(m_browser);
}

WebView::~WebView() = default;

void WebView::addJavaScript(const std::string & arg1, const std::string & arg2) {
    printf("WebView::addJavaScript(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebView::addPermissionTrustDomain(const std::string & arg1) {
    printf("WebView::addPermissionTrustDomain(%s) stub!\n", arg1.c_str());
}

void WebView::addSslExceptionDomain(const std::string & arg1) {
    printf("WebView::addSslExceptionDomain(%s) stub!\n", arg1.c_str());
}

void WebView::addUrlScheme(const std::string & arg1) {
    printf("WebView::addUrlScheme(%s) stub!\n", arg1.c_str());
}

bool WebView::animateTo(int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, float arg5, float arg6, const std::string & arg7) {
    printf("WebView::animateTo(%d, %d, %d, %d, %f, %f, %s) stub!\n", arg1, arg2, arg3, arg4, arg5, arg6, arg7.c_str());
    return true;
}

bool WebView::canGoBack() {
    return m_browser->GetBrowser()->CanGoBack();
}

bool WebView::canGoForward() {
    return m_browser->GetBrowser()->CanGoForward();
}

void WebView::cleanCache() {
    printf("WebView::cleanCache() stub!\n");
}

void WebView::evaluateJavaScript(const std::string & arg1, const std::string & arg2) {
    printf("WebView::evaluateJavaScript(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

std::string WebView::getUrl() {
    return m_browser->GetBrowser()->GetMainFrame()->GetURL();
}

std::string WebView::getUserAgent() {
    printf("WebView::getUserAgent() stub!\n");
    return "";
}

float WebView::getWebViewAlpha() {
    printf("WebView::getWebViewAlpha() stub!\n");
    return 1.0f;
}

void WebView::goBack() {
    m_browser->GetBrowser()->GoBack();
}

void WebView::goForward() {
    m_browser->GetBrowser()->GoForward();
}

bool WebView::hide(bool arg1, int32_t arg2, float arg3, const std::string & arg4) {
    printf("WebView::hide(%d, %d, %f, %s) partial stub!\n", arg1, arg2, arg3, arg4.c_str());

    /*
     * TODO: animations
     */

    m_window->SetVisible(false);

    return true;
}

void WebView::load(const std::string &url) {
    m_browser->GetBrowser()->GetMainFrame()->LoadURL(url);
}

void WebView::loadHTMLString(const std::string & arg1, const std::string & arg2) {
    printf("WebView::loadHTMLString(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebView::print() {
    m_browser->GetBrowser()->GetHost()->Print();
}

void WebView::reload() {
    m_browser->GetBrowser()->Reload();
}

void WebView::removePermissionTrustDomain(const std::string & arg1) {
    printf("WebView::removePermissionTrustDomain(%s) stub!\n", arg1.c_str());
}

void WebView::removeSslExceptionDomain(const std::string & arg1) {
    printf("WebView::removeSslExceptionDomain(%s) stub!\n", arg1.c_str());
}

void WebView::removeUrlScheme(const std::string & arg1) {
    printf("WebView::removeUrlScheme(%s) stub!\n", arg1.c_str());
}

void WebView::scrollTo(int32_t arg1, int32_t arg2, bool arg3) {
    printf("WebView::scrollTo(%d, %d, %d) stub!\n", arg1, arg2, arg3);
}

void WebView::setAllowHTTPAuthPopUpWindow(bool arg1) {
    printf("WebView::setAllowHTTPAuthPopUpWindow(%d) stub!\n", arg1);
}

void WebView::setBackButtonEnabled(bool arg1) {
    printf("WebView::setBackButtonEnabled(%d) stub!\n", arg1);
}

void WebView::setBackgroundColor(float arg1, float arg2, float arg3, float arg4) {
    printf("WebView::setBackgroundColor(%f, %f, %f, %f) stub!\n", arg1, arg2, arg3, arg4);
}

void WebView::setBouncesEnabled(bool arg1) {
    printf("WebView::setBouncesEnabled(%d) stub!\n", arg1);
}

void WebView::setCalloutEnabled(bool arg1) {
    printf("WebView::setCalloutEnabled(%d) stub!\n", arg1);
}

void WebView::setDefaultFontSize(int32_t arg1) {
    printf("WebView::setDefaultFontSize(%d) stub!\n", arg1);
}

void WebView::setHeaderField(const std::string & arg1, const std::string & arg2) {
    printf("WebView::setHeaderField(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void WebView::setHorizontalScrollBarEnabled(bool arg1) {
    printf("WebView::setHorizontalScrollBarEnabled(%d) stub!\n", arg1);
}

void WebView::setImmersiveModeEnabled(bool arg1) {
    printf("WebView::setImmersiveModeEnabled(%d) stub!\n", arg1);
}

void WebView::setLoadWithOverviewMode(bool arg1) {
    printf("WebView::setLoadWithOverviewMode(%d) stub!\n", arg1);
}

void WebView::setOpenLinksInExternalBrowser(bool arg1) {
    printf("WebView::setOpenLinksInExternalBrowser(%d) stub!\n", arg1);
}

void WebView::setShowSpinnerWhileLoading(bool arg1) {
    printf("WebView::setShowSpinnerWhileLoading(%d) stub!\n", arg1);
}

void WebView::setSpinnerText(const std::string & arg1) {
    printf("WebView::setSpinnerText(%s) stub!\n", arg1.c_str());
}

void WebView::setSupportMultipleWindows(bool arg1) {
    printf("WebView::setSupportMultipleWindows(%d) stub!\n", arg1);
}

void WebView::setUseWideViewPort(bool arg1) {
    printf("WebView::setUseWideViewPort(%d) stub!\n", arg1);
}

void WebView::setUserAgent(const std::string & arg1) {
    printf("WebView::setUserAgent(%s) stub!\n", arg1.c_str());
}

void WebView::setUserInteractionEnabled(bool arg1) {
    printf("WebView::setUserInteractionEnabled(%d) stub!\n", arg1);
}

void WebView::setVerticalScrollBarEnabled(bool arg1) {
    printf("WebView::setVerticalScrollBarEnabled(%d) stub!\n", arg1);
}

void WebView::setWebViewAlpha(float arg1) {
    printf("WebView::setWebViewAlpha(%f) stub!\n", arg1);
}

void WebView::setZoomEnabled(bool arg1) {
    printf("WebView::setZoomEnabled(%d) stub!\n", arg1);
}

bool WebView::show(bool arg1, int32_t arg2, float arg3, const std::string & arg4) {
    printf("WebView::show(%d, %d, %f, %s) partial stub!\n", arg1, arg2, arg3, arg4.c_str());

    /*
     * TODO: animations
     */

    m_window->SetVisible(true);

    return true;
}

void WebView::showWebViewDialog(bool arg1) {
    printf("WebView::showWebViewDialog(%d) stub!\n", arg1);
}

void WebView::stop() {
    m_browser->GetBrowser()->StopLoad();
}

void WebView::setFrame(int32_t x, int32_t y, int32_t width, int32_t height) {

#ifdef _WIN32
    SetWindowPos(m_window->GetWindowHandle(), nullptr, x, y, width, height, SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
#else
    auto display = cef_get_xdisplay();

    XMoveResizeWindow(display, m_window->GetWindowHandle(), x, y, width, height);
    XSync(display, True);
#endif
}

void WebView::setPosition(int32_t x, int32_t y) {
#ifdef _WIN32
    SetWindowPos(m_window->GetWindowHandle(), nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_ASYNCWINDOWPOS | SWP_NOSIZE);
#else
    auto display = cef_get_xdisplay();

    XMoveWindow(display, m_window->GetWindowHandle(), x, y);
    XSync(display, True);
#endif
}

void WebView::setSize(int32_t width, int32_t height) {
#ifdef _WIN32
    SetWindowPos(m_window->GetWindowHandle(), nullptr, 0, 0, width, height, SWP_NOZORDER |
        SWP_ASYNCWINDOWPOS | SWP_NOMOVE);
#else
    auto display = cef_get_xdisplay();

    XResizeWindow(display, m_window->GetWindowHandle(), width, height);
    XSync(display, True);
#endif
}

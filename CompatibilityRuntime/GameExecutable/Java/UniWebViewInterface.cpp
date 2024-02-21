#include <Java/UniWebViewInterface.h>
#include <Java/JNIState.h>
#include <Java/JNIString.h>

#include <WebView/WebViewImplementationFactory.h>
#include <WebView/WebViewImplementation.h>

std::unique_ptr<WebViewImplementation> UniWebViewInterface::m_impl;

std::shared_ptr<JNIClass> UniWebViewInterface::makeClass() {
    auto co = std::make_shared<JNIClass>("com/onevcat/uniwebview/UniWebViewInterface", parent("java/lang/Object"));

    co->registerMethod("addJavaScript", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::addJavaScript);
    co->registerMethod("addPermissionTrustDomain", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::addPermissionTrustDomain);
    co->registerMethod("addSslExceptionDomain", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::addSslExceptionDomain);
    co->registerMethod("addUrlScheme", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::addUrlScheme);
    co->registerMethod("animateTo", "(Ljava/lang/String;IIIIFFLjava/lang/String;)Z", &UniWebViewInterface::animateTo);
    co->registerMethod("canGoBack", "(Ljava/lang/String;)Z", &UniWebViewInterface::canGoBack);
    co->registerMethod("canGoForward", "(Ljava/lang/String;)Z", &UniWebViewInterface::canGoForward);
    co->registerMethod("cleanCache", "(Ljava/lang/String;)V", &UniWebViewInterface::cleanCache);
    co->registerMethod("clearCookies", "()V", &UniWebViewInterface::clearCookies);
    co->registerMethod("clearHttpAuthUsernamePassword", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::clearHttpAuthUsernamePassword);
    co->registerMethod("destroy", "(Ljava/lang/String;)V", &UniWebViewInterface::destroy);
    co->registerMethod("evaluateJavaScript", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::evaluateJavaScript);
    co->registerMethod("getCookie", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", &UniWebViewInterface::getCookie);
    co->registerMethod("getUrl", "(Ljava/lang/String;)Ljava/lang/String;", &UniWebViewInterface::getUrl);
    co->registerMethod("getUserAgent", "(Ljava/lang/String;)Ljava/lang/String;", &UniWebViewInterface::getUserAgent);
    co->registerMethod("getWebViewAlpha", "(Ljava/lang/String;)F", &UniWebViewInterface::getWebViewAlpha);
    co->registerMethod("goBack", "(Ljava/lang/String;)V", &UniWebViewInterface::goBack);
    co->registerMethod("goForward", "(Ljava/lang/String;)V", &UniWebViewInterface::goForward);
    co->registerMethod("hide", "(Ljava/lang/String;ZIFLjava/lang/String;)Z", &UniWebViewInterface::hide);
    co->registerMethod("init", "(Ljava/lang/String;IIII)V", &UniWebViewInterface::init);
    co->registerMethod("load", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::load);
    co->registerMethod("loadHTMLString", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::loadHTMLString);
    co->registerMethod("prepare", "()V", &UniWebViewInterface::prepare);
    co->registerMethod("print", "(Ljava/lang/String;)V", &UniWebViewInterface::print);
    co->registerMethod("reload", "(Ljava/lang/String;)V", &UniWebViewInterface::reload);
    co->registerMethod("removePermissionTrustDomain", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::removePermissionTrustDomain);
    co->registerMethod("removeSslExceptionDomain", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::removeSslExceptionDomain);
    co->registerMethod("removeUrlScheme", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::removeUrlScheme);
    co->registerMethod("screenHeight", "()F", &UniWebViewInterface::screenHeight);
    co->registerMethod("screenWidth", "()F", &UniWebViewInterface::screenWidth);
    co->registerMethod("scrollTo", "(Ljava/lang/String;IIZ)V", &UniWebViewInterface::scrollTo);
    co->registerMethod("setAllowAutoPlay", "(Z)V", &UniWebViewInterface::setAllowAutoPlay);
    co->registerMethod("setAllowHTTPAuthPopUpWindow", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setAllowHTTPAuthPopUpWindow);
    co->registerMethod("setAllowJavaScriptOpenWindow", "(Z)V", &UniWebViewInterface::setAllowJavaScriptOpenWindow);
    co->registerMethod("setBackButtonEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setBackButtonEnabled);
    co->registerMethod("setBackgroundColor", "(Ljava/lang/String;FFFF)V", &UniWebViewInterface::setBackgroundColor);
    co->registerMethod("setBouncesEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setBouncesEnabled);
    co->registerMethod("setCalloutEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setCalloutEnabled);
    co->registerMethod("setCookie", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::setCookie);
    co->registerMethod("setDefaultFontSize", "(Ljava/lang/String;I)V", &UniWebViewInterface::setDefaultFontSize);
    co->registerMethod("setFrame", "(Ljava/lang/String;IIII)V", &UniWebViewInterface::setFrame);
    co->registerMethod("setHeaderField", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::setHeaderField);
    co->registerMethod("setHorizontalScrollBarEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setHorizontalScrollBarEnabled);
    co->registerMethod("setImmersiveModeEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setImmersiveModeEnabled);
    co->registerMethod("setJavaScriptEnabled", "(Z)V", &UniWebViewInterface::setJavaScriptEnabled);
    co->registerMethod("setLoadWithOverviewMode", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setLoadWithOverviewMode);
    co->registerMethod("setLogLevel", "(I)V", &UniWebViewInterface::setLogLevel);
    co->registerMethod("setOpenLinksInExternalBrowser", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setOpenLinksInExternalBrowser);
    co->registerMethod("setPosition", "(Ljava/lang/String;II)V", &UniWebViewInterface::setPosition);
    co->registerMethod("setShowSpinnerWhileLoading", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setShowSpinnerWhileLoading);
    co->registerMethod("setSize", "(Ljava/lang/String;II)V", &UniWebViewInterface::setSize);
    co->registerMethod("setSpinnerText", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::setSpinnerText);
    co->registerMethod("setSupportMultipleWindows", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setSupportMultipleWindows);
    co->registerMethod("setUseWideViewPort", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setUseWideViewPort);
    co->registerMethod("setUserAgent", "(Ljava/lang/String;Ljava/lang/String;)V", &UniWebViewInterface::setUserAgent);
    co->registerMethod("setUserInteractionEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setUserInteractionEnabled);
    co->registerMethod("setVerticalScrollBarEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setVerticalScrollBarEnabled);
    co->registerMethod("setWebContentsDebuggingEnabled", "(Z)V", &UniWebViewInterface::setWebContentsDebuggingEnabled);
    co->registerMethod("setWebViewAlpha", "(Ljava/lang/String;F)V", &UniWebViewInterface::setWebViewAlpha);
    co->registerMethod("setZoomEnabled", "(Ljava/lang/String;Z)V", &UniWebViewInterface::setZoomEnabled);
    co->registerMethod("show", "(Ljava/lang/String;ZIFLjava/lang/String;)Z", &UniWebViewInterface::show);
    co->registerMethod("showWebViewDialog", "(Ljava/lang/String;Z)V", &UniWebViewInterface::showWebViewDialog);
    co->registerMethod("stop", "(Ljava/lang/String;)V", &UniWebViewInterface::stop);

    return co;
}

void UniWebViewInterface::prepare() {
    printf("UniWebViewInterface::prepare()\n");

    if(!m_impl)
        m_impl = WebViewImplementationFactory::createWebViewImplementation();
}

void UniWebViewInterface::addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return m_impl->addJavaScript(arg1, arg2, arg3);
}

void UniWebViewInterface::addPermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    return m_impl->addPermissionTrustDomain(arg1, arg2);
}

void UniWebViewInterface::addSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    return m_impl->addSslExceptionDomain(arg1, arg2);
}

void UniWebViewInterface::addUrlScheme(const std::string & arg1, const std::string & arg2) {
    return m_impl->addUrlScheme(arg1, arg2);
}

bool UniWebViewInterface::animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7, const std::string & arg8) {
    return m_impl->animateTo(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

bool UniWebViewInterface::canGoBack(const std::string & arg1) {
    return m_impl->canGoBack(arg1);
}

bool UniWebViewInterface::canGoForward(const std::string & arg1) {
    return m_impl->canGoForward(arg1);
}

void UniWebViewInterface::cleanCache(const std::string & arg1) {
    return m_impl->cleanCache(arg1);
}

void UniWebViewInterface::clearCookies() {
    return m_impl->clearCookies();
}

void UniWebViewInterface::clearHttpAuthUsernamePassword(const std::string & arg1, const std::string & arg2) {
    return m_impl->clearHttpAuthUsernamePassword(arg1, arg2);
}

void UniWebViewInterface::destroy(const std::string & arg1) {
    return m_impl->destroy(arg1);
}

void UniWebViewInterface::evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return m_impl->evaluateJavaScript(arg1, arg2, arg3);
}

std::shared_ptr<JNIObject> UniWebViewInterface::getCookie(const std::string & arg1, const std::string & arg2) {
    return std::make_shared<JNIString>(m_impl->getCookie(arg1, arg2));
}

std::shared_ptr<JNIObject> UniWebViewInterface::getUrl(const std::string & arg1) {
    return std::make_shared<JNIString>(m_impl->getUrl(arg1));
}

std::shared_ptr<JNIObject>  UniWebViewInterface::getUserAgent(const std::string & arg1) {
    return std::make_shared<JNIString>(arg1);
}

float UniWebViewInterface::getWebViewAlpha(const std::string & arg1) {
    return m_impl->getWebViewAlpha(arg1);
}

void UniWebViewInterface::goBack(const std::string & arg1) {
    return m_impl->goBack(arg1);
}

void UniWebViewInterface::goForward(const std::string & arg1) {
    return m_impl->goForward(arg1);
}

bool UniWebViewInterface::hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    return m_impl->hide(arg1, arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::init(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    return m_impl->init(arg1, arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::load(const std::string & arg1, const std::string & arg2) {
    return m_impl->load(arg1, arg2);
}

void UniWebViewInterface::loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return m_impl->loadHTMLString(arg1, arg2, arg3);
}

void UniWebViewInterface::print(const std::string & arg1) {
    return m_impl->print(arg1);
}

void UniWebViewInterface::reload(const std::string & arg1) {
    return m_impl->reload(arg1);
}

void UniWebViewInterface::removePermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    return m_impl->removePermissionTrustDomain(arg1, arg2);
}

void UniWebViewInterface::removeSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    return m_impl->removeSslExceptionDomain(arg1, arg2);
}

void UniWebViewInterface::removeUrlScheme(const std::string & arg1, const std::string & arg2) {
    return m_impl->removeUrlScheme(arg1, arg2);
}

float UniWebViewInterface::screenHeight() {
    return m_impl->screenHeight();
}

float UniWebViewInterface::screenWidth() {
    return m_impl->screenWidth();
}

void UniWebViewInterface::scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4) {
    return m_impl->scrollTo(arg1, arg2, arg3, arg4);
}

void UniWebViewInterface::setAllowAutoPlay(bool arg1) {
    return m_impl->setAllowAutoPlay(arg1);
}

void UniWebViewInterface::setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2) {
    return m_impl->setAllowHTTPAuthPopUpWindow(arg1, arg2);
}

void UniWebViewInterface::setAllowJavaScriptOpenWindow(bool arg1) {
    return m_impl->setAllowJavaScriptOpenWindow(arg1);
}

void UniWebViewInterface::setBackButtonEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setBackButtonEnabled(arg1, arg2);
}

void UniWebViewInterface::setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5) {
    return m_impl->setBackgroundColor(arg1, arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::setBouncesEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setBouncesEnabled(arg1, arg2);
}

void UniWebViewInterface::setCalloutEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setCalloutEnabled(arg1, arg2);
}

void UniWebViewInterface::setCookie(const std::string & arg1, const std::string & arg2) {
    return m_impl->setCookie(arg1, arg2);
}

void UniWebViewInterface::setDefaultFontSize(const std::string & arg1, int32_t arg2) {
    return m_impl->setDefaultFontSize(arg1, arg2);
}

void UniWebViewInterface::setFrame(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    return m_impl->setFrame(arg1, arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    return m_impl->setHeaderField(arg1, arg2, arg3);
}

void UniWebViewInterface::setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setHorizontalScrollBarEnabled(arg1, arg2);
}

void UniWebViewInterface::setImmersiveModeEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setImmersiveModeEnabled(arg1, arg2);
}

void UniWebViewInterface::setJavaScriptEnabled(bool arg1) {
    return m_impl->setJavaScriptEnabled(arg1);
}

void UniWebViewInterface::setLoadWithOverviewMode(const std::string & arg1, bool arg2) {
    return m_impl->setLoadWithOverviewMode(arg1, arg2);
}

void UniWebViewInterface::setLogLevel(int32_t arg1) {
    return m_impl->setLogLevel(arg1);
}

void UniWebViewInterface::setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2) {
    return m_impl->setOpenLinksInExternalBrowser(arg1, arg2);
}

void UniWebViewInterface::setPosition(const std::string & arg1, int32_t arg2, int32_t arg3) {
    return m_impl->setPosition(arg1, arg2, arg3);
}

void UniWebViewInterface::setShowSpinnerWhileLoading(const std::string & arg1, bool arg2) {
    return m_impl->setShowSpinnerWhileLoading(arg1, arg2);
}

void UniWebViewInterface::setSize(const std::string & arg1, int32_t arg2, int32_t arg3) {
    return m_impl->setSize(arg1, arg2, arg3);
}

void UniWebViewInterface::setSpinnerText(const std::string & arg1, const std::string & arg2) {
    return m_impl->setSpinnerText(arg1, arg2);
}

void UniWebViewInterface::setSupportMultipleWindows(const std::string & arg1, bool arg2) {
    return m_impl->setSupportMultipleWindows(arg1, arg2);
}

void UniWebViewInterface::setUseWideViewPort(const std::string & arg1, bool arg2) {
    return m_impl->setUseWideViewPort(arg1, arg2);
}

void UniWebViewInterface::setUserAgent(const std::string & arg1, const std::string & arg2) {
    return m_impl->setUserAgent(arg1, arg2);
}

void UniWebViewInterface::setUserInteractionEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setUserInteractionEnabled(arg1, arg2);
}

void UniWebViewInterface::setVerticalScrollBarEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setVerticalScrollBarEnabled(arg1, arg2);
}

void UniWebViewInterface::setWebContentsDebuggingEnabled(bool arg1) {
    return m_impl->setWebContentsDebuggingEnabled(arg1);
}

void UniWebViewInterface::setWebViewAlpha(const std::string & arg1, float arg2) {
    return m_impl->setWebViewAlpha(arg1, arg2);
}

void UniWebViewInterface::setZoomEnabled(const std::string & arg1, bool arg2) {
    return m_impl->setZoomEnabled(arg1, arg2);
}

bool UniWebViewInterface::show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    return m_impl->show(arg1, arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::showWebViewDialog(const std::string & arg1, bool arg2) {
    return m_impl->showWebViewDialog(arg1, arg2);
}

void UniWebViewInterface::stop(const std::string & arg1) {
    return m_impl->stop(arg1);
}



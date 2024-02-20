#include <Java/UniWebViewInterface.h>
#include <Java/JNIState.h>
#include <Java/JNIString.h>

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

void UniWebViewInterface::addJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("UniWebViewInterface::addJavaScript(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void UniWebViewInterface::addPermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::addPermissionTrustDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::addSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::addSslExceptionDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::addUrlScheme(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::addUrlScheme(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

bool UniWebViewInterface::animateTo(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, float arg6, float arg7, const std::string & arg8) {
    printf("UniWebViewInterface::animateTo(%s, %d, %d, %d, %d, %f, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5, arg6, arg7, arg8.c_str());
    return true;
}

bool UniWebViewInterface::canGoBack(const std::string & arg1) {
    printf("UniWebViewInterface::canGoBack(%s) stub!\n", arg1.c_str());
    return false;
}

bool UniWebViewInterface::canGoForward(const std::string & arg1) {
    printf("UniWebViewInterface::canGoForward(%s) stub!\n", arg1.c_str());
    return false;
}

void UniWebViewInterface::cleanCache(const std::string & arg1) {
    printf("UniWebViewInterface::cleanCache(%s) stub!\n", arg1.c_str());
}

void UniWebViewInterface::clearCookies() {
    printf("UniWebViewInterface::clearCookies() stub!\n");
}

void UniWebViewInterface::clearHttpAuthUsernamePassword(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::clearHttpAuthUsernamePassword(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::destroy(const std::string & arg1) {
    printf("UniWebViewInterface::destroy(%s) stub!\n", arg1.c_str());
}

void UniWebViewInterface::evaluateJavaScript(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("UniWebViewInterface::evaluateJavaScript(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

std::shared_ptr<JNIObject> UniWebViewInterface::getCookie(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::getCookie(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());

    return std::make_shared<JNIString>();
}

std::shared_ptr<JNIObject> UniWebViewInterface::getUrl(const std::string & arg1) {
    printf("UniWebViewInterface::getUrl(%s) stub!\n", arg1.c_str());
    return std::make_shared<JNIString>();
}

std::shared_ptr<JNIObject> UniWebViewInterface::getUserAgent(const std::string & arg1) {
    printf("UniWebViewInterface::getUserAgent(%s) stub!\n", arg1.c_str());
    return std::make_shared<JNIString>();
}

float UniWebViewInterface::getWebViewAlpha(const std::string & arg1) {
    printf("UniWebViewInterface::getWebViewAlpha(%s) stub!\n", arg1.c_str());
    return 1.0f;
}

void UniWebViewInterface::goBack(const std::string & arg1) {
    printf("UniWebViewInterface::goBack(%s) stub!\n", arg1.c_str());
}

void UniWebViewInterface::goForward(const std::string & arg1) {
    printf("UniWebViewInterface::goForward(%s) stub!\n", arg1.c_str());
}

bool UniWebViewInterface::hide(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    printf("UniWebViewInterface::hide(%s, %d, %d, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5.c_str());
    return true;
}

void UniWebViewInterface::init(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    printf("UniWebViewInterface::init(%s, %d, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::load(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::load(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::loadHTMLString(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("UniWebViewInterface::loadHTMLString(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void UniWebViewInterface::prepare() {
    printf("UniWebViewInterface::prepare() stub!\n");
}

void UniWebViewInterface::print(const std::string & arg1) {
    printf("UniWebViewInterface::print(%s) stub!\n", arg1.c_str());
}

void UniWebViewInterface::reload(const std::string & arg1) {
    printf("UniWebViewInterface::reload(%s) stub!\n", arg1.c_str());
}

void UniWebViewInterface::removePermissionTrustDomain(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::removePermissionTrustDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::removeSslExceptionDomain(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::removeSslExceptionDomain(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::removeUrlScheme(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::removeUrlScheme(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

float UniWebViewInterface::screenHeight() {
    printf("UniWebViewInterface::screenHeight() stub!\n");
    return 640.0f;
}

float UniWebViewInterface::screenWidth() {
    printf("UniWebViewInterface::screenWidth() stub!\n");
    return 480.0f;
}

void UniWebViewInterface::scrollTo(const std::string & arg1, int32_t arg2, int32_t arg3, bool arg4) {
    printf("UniWebViewInterface::scrollTo(%s, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4);
}

void UniWebViewInterface::setAllowAutoPlay(bool arg1) {
    printf("UniWebViewInterface::setAllowAutoPlay(%d) stub!\n", arg1);
}

void UniWebViewInterface::setAllowHTTPAuthPopUpWindow(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setAllowHTTPAuthPopUpWindow(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setAllowJavaScriptOpenWindow(bool arg1) {
    printf("UniWebViewInterface::setAllowJavaScriptOpenWindow(%d) stub!\n", arg1);
}

void UniWebViewInterface::setBackButtonEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setBackButtonEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setBackgroundColor(const std::string & arg1, float arg2, float arg3, float arg4, float arg5) {
    printf("UniWebViewInterface::setBackgroundColor(%s, %f, %f, %f, %f) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::setBouncesEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setBouncesEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setCalloutEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setCalloutEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setCookie(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::setCookie(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::setDefaultFontSize(const std::string & arg1, int32_t arg2) {
    printf("UniWebViewInterface::setDefaultFontSize(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setFrame(const std::string & arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) {
    printf("UniWebViewInterface::setFrame(%s, %d, %d, %d, %d) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5);
}

void UniWebViewInterface::setHeaderField(const std::string & arg1, const std::string & arg2, const std::string & arg3) {
    printf("UniWebViewInterface::setHeaderField(%s, %s, %s) stub!\n", arg1.c_str(), arg2.c_str(), arg3.c_str());
}

void UniWebViewInterface::setHorizontalScrollBarEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setHorizontalScrollBarEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setImmersiveModeEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setImmersiveModeEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setJavaScriptEnabled(bool arg1) {
    printf("UniWebViewInterface::setJavaScriptEnabled(%d) stub!\n", arg1);
}

void UniWebViewInterface::setLoadWithOverviewMode(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setLoadWithOverviewMode(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setLogLevel(int32_t arg1) {
    printf("UniWebViewInterface::setLogLevel(%d) stub!\n", arg1);
}

void UniWebViewInterface::setOpenLinksInExternalBrowser(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setOpenLinksInExternalBrowser(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setPosition(const std::string & arg1, int32_t arg2, int32_t arg3) {
    printf("UniWebViewInterface::setPosition(%s, %d, %d) stub!\n", arg1.c_str(), arg2, arg3);
}

void UniWebViewInterface::setShowSpinnerWhileLoading(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setShowSpinnerWhileLoading(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setSize(const std::string & arg1, int32_t arg2, int32_t arg3) {
    printf("UniWebViewInterface::setSize(%s, %d, %d) stub!\n", arg1.c_str(), arg2, arg3);
}

void UniWebViewInterface::setSpinnerText(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::setSpinnerText(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::setSupportMultipleWindows(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setSupportMultipleWindows(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setUseWideViewPort(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setUseWideViewPort(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setUserAgent(const std::string & arg1, const std::string & arg2) {
    printf("UniWebViewInterface::setUserAgent(%s, %s) stub!\n", arg1.c_str(), arg2.c_str());
}

void UniWebViewInterface::setUserInteractionEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setUserInteractionEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setVerticalScrollBarEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setVerticalScrollBarEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setWebContentsDebuggingEnabled(bool arg1) {
    printf("UniWebViewInterface::setWebContentsDebuggingEnabled(%d) stub!\n", arg1);
}

void UniWebViewInterface::setWebViewAlpha(const std::string & arg1, float arg2) {
    printf("UniWebViewInterface::setWebViewAlpha(%s, %f) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::setZoomEnabled(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::setZoomEnabled(%s, %d) stub!\n", arg1.c_str(), arg2);
}

bool UniWebViewInterface::show(const std::string & arg1, bool arg2, int32_t arg3, float arg4, const std::string & arg5) {
    printf("UniWebViewInterface::show(%s, %d, %d, %f, %s) stub!\n", arg1.c_str(), arg2, arg3, arg4, arg5.c_str());
    return true;
}

void UniWebViewInterface::showWebViewDialog(const std::string & arg1, bool arg2) {
    printf("UniWebViewInterface::showWebViewDialog(%s, %d) stub!\n", arg1.c_str(), arg2);
}

void UniWebViewInterface::stop(const std::string & arg1) {
    printf("UniWebViewInterface::stop(%s) stub!\n", arg1.c_str());
}


#include <WebView/WebViewImplementationFactory.h>
#include <WebView/WebViewImplementation.h>

#if defined(BUILDING_WITH_CEF)
#include <WebView/CEFWebViewImplementation.h>
#endif

std::unique_ptr<WebViewImplementation> WebViewImplementationFactory::createWebViewImplementation() {

#if defined(BUILDING_WITH_CEF)
    return std::make_unique<CEFWebViewImplementation>();
#else
    /*
     * Dummy implementation.
     */
    return std::make_unique<WebViewImplementation>();
#endif

}

#include <WebView/WebViewImplementationFactory.h>
#include <WebView/WebViewImplementation.h>

#if defined(BUILDING_WITH_CEF)
#include <WebView/CEFWebViewImplementation.h>
#include <WebViewHostClientConfiguration.h>
#endif

std::unique_ptr<WebViewImplementation> WebViewImplementationFactory::createWebViewImplementation() {

#if defined(BUILDING_WITH_CEF)
    WebViewHostClientConfiguration config;
    if(config.isUsable()) {
        return std::make_unique<CEFWebViewImplementation>(config);
    }
#endif
    /*
     * Dummy implementation.
     */
    return std::make_unique<WebViewImplementation>();

}

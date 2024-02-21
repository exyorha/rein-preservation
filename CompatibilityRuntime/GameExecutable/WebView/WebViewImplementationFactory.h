#ifndef WEB_VIEW_WEB_VIEW_IMPLEMENTATION_FACTORY_H
#define WEB_VIEW_WEB_VIEW_IMPLEMENTATION_FACTORY_H

#include <memory>

class WebViewImplementation;

class WebViewImplementationFactory {
public:
    static std::unique_ptr<WebViewImplementation> createWebViewImplementation();


};

#endif

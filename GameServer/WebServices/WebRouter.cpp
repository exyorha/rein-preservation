#include "WebServices/WebRouter.h"
#include "WebServices/WebRoutable.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/URI.h"

WebRouter::WebRouter(WebRoutable *defaultRoutable) : m_defaultRoutable(defaultRoutable) {

}

WebRouter::~WebRouter() = default;

void WebRouter::handle(LLServices::HttpRequest &&request) {

    auto path = request.uri().path();
    if(!path.has_value() || path->empty() || path->front() != '/') {
        request.sendError(404, "Not Found");
        return;
    }

    if(path == "/")
        path = "/database";

    auto cleanPath = *path;

    for(const auto &subpath: m_subpaths) {
        if(cleanPath.starts_with(subpath.subpath) &&
            (cleanPath.size() == subpath.subpath.size() || cleanPath[subpath.subpath.size()] == '/')) {

            cleanPath = cleanPath.substr(subpath.subpath.size());
            if(cleanPath.empty())
                cleanPath = "/";

            return subpath.routable->handle(cleanPath, std::move(request));
        }
    }

    m_defaultRoutable->handle(cleanPath, std::move(request));
}

void WebRouter::handleSubpath(std::string &&subpath, WebRoutable *routable) {
    m_subpaths.emplace_back(SubpathRoutable{ .subpath = std::move(subpath), .routable = routable });
}


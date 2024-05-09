#include "WebServices/WebRedirector.h"

#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/KeyValuePairs.h"
#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/URI.h"

WebRedirector::WebRedirector() = default;

WebRedirector::~WebRedirector() = default;

void WebRedirector::handle(const std::string_view &path, LLServices::HttpRequest &&request) {

    if(request.command() != EVHTTP_REQ_GET && request.command() != EVHTTP_REQ_HEAD) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    auto uri = request.uri();
    uri.setScheme(std::nullopt);
    uri.setUserInfo(std::nullopt);
    uri.setHost(std::nullopt);
    uri.setPort(std::nullopt);
    uri.setPath(std::string(path));

    auto headers = request.outputHeaders();
    headers.add("Location", uri.join().c_str());

    LLServices::Buffer buffer;
    request.sendReply(301, "Moved Permanently", buffer);
}

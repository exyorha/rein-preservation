#include "DatabaseViewer/DatabaseViewerResponse.h"

#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/KeyValuePairs.h"

DatabaseViewerResponse::DatabaseViewerResponse(LLServices::HttpRequest &&request) :m_request(std::move(request)) {
    m_response.setRoot(XMLNode("database-page"));
}

DatabaseViewerResponse::~DatabaseViewerResponse() = default;

void DatabaseViewerResponse::finish() {
    m_request.outputHeaders().add("Content-Type", "text/xml");

    std::string xml(
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<?xml-stylesheet href=\"/style.xsl\" type=\"text/xsl\"?>\n");

    m_response.writeXML(xml);

    LLServices::Buffer buffer;
    buffer.append(xml);
    m_request.sendReply(200, "OK", buffer);
}

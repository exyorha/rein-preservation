#include "OctoWebServices.h"

#include <ClientDataAccess/OctoContentStorage.h>

#include "LLServices/Logging/LogFacility.h"

#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/KeyValuePairs.h"

#include "GRPC/PBOutputOverLLBuffer.h"

#include <regex>
#include <charconv>

LLServices::LogFacility LogOctoWebServices("OctoWebServices");

OctoWebServices::OctoWebServices(const ClientDataAccess::OctoContentStorage &octoStorage) : m_storage(octoStorage) {

    LogOctoWebServices.info("Serving Octo API requests using the database with the revision %d", m_storage.database().revision());
}

OctoWebServices::~OctoWebServices() = default;

void OctoWebServices::handle(const std::string_view &path, LLServices::HttpRequest &&request) {
    /*
     * Other known endpoints:
     *
     * - /v1/er - error reporting, POSTs a JSON object detailing a .NET
     *   exception in Octo code.
     */

    static const std::regex listAccess("/v2/pub/a/([0-9]+)/v/([0-9]+)/list/([0-9]+)");

    std::cmatch segments;

    if(std::regex_match(path.data(), path.data() + path.size(), segments, listAccess)) {

        auto appId = parseNumber(segments[1]);
        auto appVersion = parseNumber(segments[2]);
        auto currentRevision = parseNumber(segments[3]);

        if(request.command() != EVHTTP_REQ_GET) {
            request.sendError(405, "Method Not Allowed");
            return;
        }

        LogOctoWebServices.debug("List request: app ID %u, app version %u, current list revision %u\n",
                                 appId, appVersion, currentRevision);

        auto buffer = handleListRequest(appId, appVersion, currentRevision);

        request.outputHeaders().add("Content-Type", "application/x-protobuf");

        request.sendReply(200, "OK", buffer);
    } else {

        request.sendError(404, "Not Found");
    }
}

unsigned int OctoWebServices::parseNumber(const std::sub_match<const char *> &match) {
    if(!match.matched)
        throw std::runtime_error("group not captured");

    return parseNumber(std::string_view(match.first, match.second));
}

unsigned int OctoWebServices::parseNumber(const std::string_view &piece) {
    unsigned int value;

    auto result = std::from_chars(piece.data(), piece.data() + piece.size(), value, 10);
    if(result.ec != std::errc() || result.ptr != piece.data() + piece.size() || piece.empty())
        throw std::runtime_error("failed to parse the range value");

    return value;
}

LLServices::Buffer OctoWebServices::handleListRequest(unsigned int appId, unsigned int version, unsigned int currentListRevision) {
    LLServices::Buffer buffer;
    PBOutputOverLLBuffer out(buffer.get());

    if(currentListRevision == m_storage.database().revision()) {
        Octo::Proto::Database upToDateResponse;
        upToDateResponse.set_revision(m_storage.database().revision());
        upToDateResponse.set_urlformat(m_storage.database().urlformat());
        if(!upToDateResponse.SerializeToZeroCopyStream(&out))
            throw std::runtime_error("Failed to serialize the Octo DB");

    } else {
        if(!m_storage.database().SerializeToZeroCopyStream(&out))
            throw std::runtime_error("Failed to serialize the Octo DB");
    }

    return buffer;
}


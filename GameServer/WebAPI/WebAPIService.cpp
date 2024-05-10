#include "WebAPI/WebAPIService.h"

#include "LLServices/JSON/JSONWriter.h"
#include "LLServices/Networking/Buffer.h"
#include "LLServices/Networking/HttpRequest.h"
#include "LLServices/Networking/KeyValuePairs.h"

#include "LLServices/JSON/JSONTree.h"

#include "DataModel/Database.h"
#include "DataModel/Sqlite/Statement.h"

using namespace std::string_view_literals;

const WebAPIService::PathHandler WebAPIService::m_pathHandlers[]{
    { .path = "/api/information/list/get"sv, .handler = &WebAPIService::informationListGet },
    { .path = "/api/information/detail/get"sv, .handler = &WebAPIService::informationDetailGet },
    { .path = "/api/information/banner/list/get"sv, .handler = &WebAPIService::bannerListGet },

};

WebAPIService::WebAPIService(Database &db, const std::filesystem::path &resourcePath) : m_db(db),
    m_webDB(resourcePath / "information.sql", m_db) {

}

WebAPIService::~WebAPIService() = default;

void WebAPIService::handle(const std::string_view &routedPath, LLServices::HttpRequest &&request) {
    if(request.command() != EVHTTP_REQ_POST) {
        request.sendError(405, "Method Not Allowed");
        return;
    }

    std::string input;
    input.resize(evbuffer_get_length(request.inputBuffer()));
    evbuffer_remove(request.inputBuffer(), input.data(), input.size());

    auto requestBody = LLServices::JSONTree::parse(input);

    m_webDB.reloadIfNeeded();

    for(const auto &handler: m_pathHandlers) {
        if(handler.path == routedPath) {

            LLServices::JSONWriter response;

            (this->*handler.handler)(std::move(requestBody), response);

            LLServices::Buffer buffer;
            buffer.append(response.output());
            request.outputHeaders().add("Content-Type", "application/json");
            request.outputHeaders().add("Cache-Control", "no-store");
            request.sendReply(200, "OK", buffer);

            return;
        }
    }

    request.sendError(404, "Not Found");
}

void WebAPIService::informationListGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response) {

    std::stringstream query;
    query << R"SQL(
        SELECT
            informationId,
            webViewMissionId,
            informationType,
            title,
            publishStartDatetime,
            postscriptDatetime,
            thumbnailImagePath
        FROM web.information
        WHERE informationType IN ()SQL";

    const char *pathInformationType[]{ "informationTypeList", nullptr };
    auto typeList = yajl_tree_get(requestBody, pathInformationType, yajl_t_array);
    if(!typeList)
        throw std::runtime_error("required informationTypeList wasn't specified in the request");

    auto typeListArray = YAJL_GET_ARRAY(typeList);

    for(size_t index = 0; index < typeListArray->len; index++) {
        if(index > 0)
            query << ", ";

        query << "?";
    }

    query << R"SQL(
        )
        ORDER BY COALESCE(postscriptDatetime, publishStartDatetime) DESC
        LIMIT ? OFFSET ? - 1
    )SQL";

    auto statement = m_db.db().prepare(query.str());
    for(size_t index = 0; index < typeListArray->len; index++) {

        if(!YAJL_IS_INTEGER(typeListArray->values[index]))
            throw std::runtime_error("non-integer entry in informationTypeList");

        statement->bind(index + 1, static_cast<int64_t>(YAJL_GET_INTEGER(typeListArray->values[index])));
    }

    const char *pathLimit[]{ "limit", nullptr };
    auto limit = yajl_tree_get(requestBody, pathLimit, yajl_t_number);
    if(!limit)
        throw std::runtime_error("required limit wasn't specified in the request");

    statement->bind(typeListArray->len + 1, static_cast<int64_t>(YAJL_GET_INTEGER(limit)));

    const char *pathOffset[]{ "offset", nullptr };
    auto offset = yajl_tree_get(requestBody, pathOffset, yajl_t_number);
    if(!offset)
        throw std::runtime_error("required offset wasn't specified in the request");

    statement->bind(typeListArray->len + 2, static_cast<int64_t>(YAJL_GET_INTEGER(offset)));

    response.writeMapOpen();

    response.writeString("commonResponse");
    response.writeMapOpen();
    response.writeString("responseDatetime");
    response.writeInteger(m_db.serverTime());
    response.writeMapClose();

    response.writeString("informationList");
    response.writeArrayOpen();

    while(statement->step()) {
        response.writeMapOpen();

        response.writeString("informationId");
        response.writeInteger(statement->columnInt64(0));

        response.writeString("webViewMissionId");
        response.writeInteger(statement->columnInt64(1));

        response.writeString("informationType");
        response.writeInteger(statement->columnInt64(2));

        response.writeString("title");
        response.writeString(statement->columnText(3));

        response.writeString("publishStartDatetime");
        response.writeInteger(statement->columnInt64(4));

        if(statement->columnType(5) != SQLITE_NULL) {

            response.writeString("postscriptDatetime");
            response.writeInteger(statement->columnInt64(5));
        }

        if(statement->columnType(6) != SQLITE_NULL) {

            response.writeString("thumbnailImagePath");
            response.writeString(statement->columnText(6));
        }

        response.writeMapClose();
    }

    response.writeArrayClose();

    response.writeMapClose();

}

void WebAPIService::informationDetailGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response) {
    auto query = m_db.db().prepare(
        "SELECT informationType, title, body, publishStartDatetime, postscriptDatetime FROM web.information WHERE informationId = ?"
    );

    const char *pathInformationId[]{ "informationId", nullptr };
    auto informationId = yajl_tree_get(requestBody, pathInformationId, yajl_t_number);
    if(!informationId)
        throw std::runtime_error("required informationId wasn't specified in the request");

    query->bind(1, static_cast<int64_t>(YAJL_GET_INTEGER(informationId)));

    if(!query->step())
        throw std::runtime_error("no such information item");

    response.writeMapOpen();

    response.writeString("commonResponse");
    response.writeMapOpen();
    response.writeString("responseDatetime");
    response.writeInteger(m_db.serverTime());
    response.writeMapClose();

    response.writeString("informationType");
    response.writeInteger(query->columnInt64(0));

    response.writeString("title");
    response.writeString(query->columnText(1));

    response.writeString("body");
    response.writeString(query->columnText(2));

    response.writeString("publishStartDatetime");
    response.writeInteger(query->columnInt64(3));

    if(query->columnType(4) != SQLITE_NULL) {

        response.writeString("postscriptDatetime");
        response.writeInteger(query->columnInt64(4));

    }

    response.writeMapClose();
}

void WebAPIService::bannerListGet(LLServices::JSONTree &&requestBody, LLServices::JSONWriter &response) {
    /*
     * It appears that the backend implementation of this service doesn't
     * implement pagination, even though the client sends limit/offset
     */

    std::stringstream query;
    query << R"SQL(
        SELECT
            informationId,
            bannerImagePath,
            webViewMissionId,
            informationType
        FROM web.information
        WHERE informationType IN ()SQL";

    const char *pathInformationType[]{ "informationTypeList", nullptr };
    auto typeList = yajl_tree_get(requestBody, pathInformationType, yajl_t_array);
    if(!typeList)
        throw std::runtime_error("required informationTypeList wasn't specified in the request");

    auto typeListArray = YAJL_GET_ARRAY(typeList);

    for(size_t index = 0; index < typeListArray->len; index++) {
        if(index > 0)
            query << ", ";

        query << "?";
    }

    query << R"SQL(
        ) AND bannerImagePath IS NOT NULL
        ORDER BY COALESCE(postscriptDatetime, publishStartDatetime) DESC
    )SQL";

    auto statement = m_db.db().prepare(query.str());
    for(size_t index = 0; index < typeListArray->len; index++) {

        if(!YAJL_IS_INTEGER(typeListArray->values[index]))
            throw std::runtime_error("non-integer entry in informationTypeList");

        statement->bind(index + 1, static_cast<int64_t>(YAJL_GET_INTEGER(typeListArray->values[index])));
    }

    response.writeMapOpen();

    response.writeString("commonResponse");
    response.writeMapOpen();
    response.writeString("responseDatetime");
    response.writeInteger(m_db.serverTime());
    response.writeMapClose();

    response.writeString("bannerList");
    response.writeArrayOpen();

    while(statement->step()) {

        response.writeMapOpen();

        response.writeString("informationId");
        response.writeInteger(statement->columnInt64(0));

        response.writeString("bannerImagePath");
        response.writeString(statement->columnText(1));

        response.writeString("webViewMissionId");
        response.writeInteger(statement->columnInt64(2));

        response.writeString("informationType");
        response.writeInteger(statement->columnInt64(3));

        response.writeMapClose();
    }

    response.writeArrayClose();
    response.writeMapClose();
}

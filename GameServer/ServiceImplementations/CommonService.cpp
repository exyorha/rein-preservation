#include <ServiceImplementations/CommonService.h>

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

#include <DataModel/DatabaseEnums.h>

#include <cstdio>

#include <chrono>

CommonService::CommonService(Database &db) : m_db(db) {

}

CommonService::~CommonService() = default;

::grpc::Status CommonService::guardedCall(const char *callName, const std::function<void()> &body) {
    try {

        body();

        return grpc::Status::OK;
    } catch(const std::exception &e) {

        fprintf(stderr, "An exception occurred while processing %s: %s\n", callName, e.what());

        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());

    } catch(...) {
        fprintf(stderr, "An exception occurred while processing %s: unknown\n", callName);
        return grpc::Status(grpc::StatusCode::INTERNAL, "unknown");

    }
}

int64_t CommonService::authenticate(::grpc::ServerContext *context) {
    const auto &metadata = context->client_metadata();
    auto it = metadata.find("x-apb-session-key");
    if(it == metadata.end())
        throw std::runtime_error("'x-apb-session-key' not specified");

    const auto &session = it->second;

    auto result = m_db.authenticate(std::string_view(session.begin(), session.end()));
    if(result.has_value())
        return *result;

    throw std::runtime_error("bad session key");
}

void CommonService::addDateToContext(::grpc::ServerContext* context) {
    context->AddTrailingMetadata(
        "x-apb-response-datetime",
        std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
    );
}

#include <ServiceImplementations/CommonService.h>

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

#include <cstdio>

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

    auto statement = db().prepare("SELECT user_id FROM internal_sessions WHERE session_id = ? AND expire_datetime >= unixepoch('now')");
    statement->bind(1, std::string(session.begin(), session.end()));

    if(statement->step()) {
        return statement->columnInt64(0);
    }

    throw std::runtime_error("bad session key");
}

#include <ServiceImplementations/CommonService.h>

#include <GRPC/GRPCLikeCall.h>

#include <LLServices/Networking/KeyValuePairs.h>

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

#include <DataModel/DatabaseEnums.h>

#include <cstdio>

LLServices::LogFacility LogRPC("RPC");

CommonService::CommonService(Database &db, const GameServerConfiguration &config) : m_db(db, config) {

}

CommonService::~CommonService() = default;

void CommonService::guardedCall(const char *callName,
        ::google::protobuf::RpcController* controller,
        ::google::protobuf::Closure* done,
        const std::function<void()> &body) {
    try {

        body();
    } catch(const std::exception &e) {

        LogRPC.error("An exception occurred while processing %s: %s\n", callName, e.what());

        controller->SetFailed(e.what());

    } catch(...) {
        LogRPC.error("An exception occurred while processing %s: unknown\n", callName);

        controller->SetFailed("no message available");

    }

    done->Run();
}

int64_t CommonService::authenticate(::google::protobuf::RpcController*controller) {
    auto header = static_cast<GRPCLikeCall *>(controller)->httpRequest().inputHeaders().find("x-apb-session-key");
    if(!header)
        throw std::runtime_error("'x-apb-session-key' not specified");

    auto result = m_db.authenticate(header);
    if(result.has_value())
        return *result;

    throw std::runtime_error("bad session key");
}

void CommonService::addDateToContext(::google::protobuf::RpcController *controller) {
    static_cast<GRPCLikeCall *>(controller)->httpRequest().outputHeaders().add("x-apb-response-datetime",
                                                                               std::to_string(m_db.dataModel().serverTime()).c_str());
}


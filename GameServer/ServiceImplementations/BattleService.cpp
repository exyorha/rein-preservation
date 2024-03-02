#include <ServiceImplementations/BattleService.h>

#include <DataModel/Sqlite/Statement.h>

BattleService::BattleService(Database &db) : CommonService(db) {

}

BattleService::~BattleService() = default;

void BattleService::StartWave(::google::protobuf::RpcController* controller,
                        const ::apb::api::battle::StartWaveRequest* request,
                        ::apb::api::battle::StartWaveResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("BattleService::StartWave", controller, request, response, done, &BattleService::StartWaveImpl);
}

void BattleService::StartWaveImpl(
    UserContext &user, const ::apb::api::battle::StartWaveRequest* request,
    ::apb::api::battle::StartWaveResponse* response) {

}

void BattleService::FinishWave(::google::protobuf::RpcController* controller,
                        const ::apb::api::battle::FinishWaveRequest* request,
                        ::apb::api::battle::FinishWaveResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("BattleService::FinishWave", controller, request, response, done, &BattleService::FinishWaveImpl);
}

void BattleService::FinishWaveImpl(
    UserContext &user, const ::apb::api::battle::FinishWaveRequest* request,
    ::apb::api::battle::FinishWaveResponse* response) {

}

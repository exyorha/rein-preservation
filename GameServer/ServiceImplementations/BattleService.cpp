#include <ServiceImplementations/BattleService.h>

#include <DataModel/Sqlite/Statement.h>

BattleService::BattleService(Database &db) : CommonService(db) {

}

BattleService::~BattleService() = default;

::grpc::Status BattleService::StartWave(
    ::grpc::ServerContext* context, const ::apb::api::battle::StartWaveRequest* request,
    ::apb::api::battle::StartWaveResponse* response) {

    return inChangesetCall("BattleService::StartWave", context, request, response, &BattleService::StartWaveImpl);
}

void BattleService::StartWaveImpl(
    UserContext &user, const ::apb::api::battle::StartWaveRequest* request,
    ::apb::api::battle::StartWaveResponse* response) {

}

::grpc::Status BattleService::FinishWave(
    ::grpc::ServerContext* context, const ::apb::api::battle::FinishWaveRequest* request,
    ::apb::api::battle::FinishWaveResponse* response) {

    return inChangesetCall("BattleService::FinishWave", context, request, response, &BattleService::FinishWaveImpl);
}

void BattleService::FinishWaveImpl(
    UserContext &user, const ::apb::api::battle::FinishWaveRequest* request,
    ::apb::api::battle::FinishWaveResponse* response) {

}

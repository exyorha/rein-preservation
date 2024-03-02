#include "GimmickService.h"

#include <cstdio>

GimmickService::GimmickService(Database &db) : CommonService(db) {

}

GimmickService::~GimmickService() = default;

void GimmickService::InitSequenceSchedule(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gimmick::InitSequenceScheduleResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("GimmickService::InitSequenceSchedule", controller, request, response,
                           done, &GimmickService::InitSequenceScheduleImpl);
}

void GimmickService::InitSequenceScheduleImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) {

}


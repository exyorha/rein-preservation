#include "GimmickService.h"

#include <cstdio>

GimmickService::GimmickService(Database &db) : CommonService(db) {

}

GimmickService::~GimmickService() = default;

::grpc::Status GimmickService::InitSequenceSchedule(::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) {

    return inChangesetCall("GimmickService::InitSequenceSchedule", context, request, response, &GimmickService::InitSequenceScheduleImpl);
}

void GimmickService::InitSequenceScheduleImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) {

}


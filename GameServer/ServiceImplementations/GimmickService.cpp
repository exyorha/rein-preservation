#include "GimmickService.h"

#include <cstdio>

GimmickService::GimmickService(Database &db) : CommonService(db) {

}

GimmickService::~GimmickService() = default;

::grpc::Status GimmickService::InitSequenceSchedule(::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) {

    printf("GimmickService::InitSequenceSchedule\n");

    return grpc::Status::OK;
}

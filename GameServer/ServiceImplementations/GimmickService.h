#ifndef SERVICE_IMPLEMENTATIONS_GIMMICK_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GIMMICK_SERVICE_H

#include <service/GimmickService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class GimmickService final : public apb::api::gimmick::GimmickService::Service, public CommonService {
public:
    explicit GimmickService(Database &db);
    ~GimmickService();

    GimmickService(const GimmickService &other) = delete;
    GimmickService &operator =(const GimmickService &other) = delete;

    ::grpc::Status InitSequenceSchedule(::grpc::ServerContext* context,
                                        const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) override;

};

#endif


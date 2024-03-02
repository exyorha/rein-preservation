#ifndef SERVICE_IMPLEMENTATIONS_GIMMICK_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GIMMICK_SERVICE_H

#include <service/GimmickService.pb.h>

#include <ServiceImplementations/CommonService.h>

class GimmickService final : public apb::api::gimmick::GimmickService, public CommonService {
public:
    explicit GimmickService(Database &db);
    ~GimmickService();

    GimmickService(const GimmickService &other) = delete;
    GimmickService &operator =(const GimmickService &other) = delete;

    void InitSequenceSchedule(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gimmick::InitSequenceScheduleResponse* response,
                            ::google::protobuf::Closure* done) override;

private:

    void InitSequenceScheduleImpl(
        UserContext &user,
        const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response);

};

#endif


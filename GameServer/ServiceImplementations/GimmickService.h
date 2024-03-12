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

    void UpdateSequence(::google::protobuf::RpcController* controller,
                        const ::apb::api::gimmick::UpdateSequenceRequest* request,
                        ::apb::api::gimmick::UpdateSequenceResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateGimmickProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::gimmick::UpdateGimmickProgressRequest* request,
                        ::apb::api::gimmick::UpdateGimmickProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void InitSequenceSchedule(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gimmick::InitSequenceScheduleResponse* response,
                            ::google::protobuf::Closure* done) override;

    void Unlock(::google::protobuf::RpcController* controller,
                        const ::apb::api::gimmick::UnlockRequest* request,
                        ::apb::api::gimmick::UnlockResponse* response,
                        ::google::protobuf::Closure* done) override;
private:

    void UpdateSequenceImpl(UserContext &user,
                            const ::apb::api::gimmick::UpdateSequenceRequest* request,
                            ::apb::api::gimmick::UpdateSequenceResponse* response);

    void UpdateGimmickProgressImpl(UserContext &user,
                                   const ::apb::api::gimmick::UpdateGimmickProgressRequest* request,
                                   ::apb::api::gimmick::UpdateGimmickProgressResponse* response);

    void InitSequenceScheduleImpl(
        UserContext &user,
        const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response);

    void UnlockImpl(UserContext &user,
                    const ::apb::api::gimmick::UnlockRequest* request,
                    ::apb::api::gimmick::UnlockResponse* response);
};

#endif


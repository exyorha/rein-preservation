#ifndef SERVICE_IMPLEMENTATIONS_COSTUME_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COSTUME_SERVICE_H

#include <service/CostumeService.pb.h>

#include <ServiceImplementations/CommonService.h>

class CostumeService final : public apb::api::costume::CostumeService, public CommonService {
public:
    explicit CostumeService(Database &db);
    ~CostumeService();

    CostumeService(const CostumeService &other) = delete;
    CostumeService &operator =(const CostumeService &other) = delete;

    void Enhance(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::EnhanceRequest* request,
                        ::apb::api::costume::EnhanceResponse* response,
                        ::google::protobuf::Closure* done) override;

    void LimitBreak(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::LimitBreakRequest* request,
                        ::apb::api::costume::LimitBreakResponse* response,
                        ::google::protobuf::Closure* done) override;

    void EnhanceActiveSkill(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::EnhanceActiveSkillRequest* request,
                        ::apb::api::costume::EnhanceActiveSkillResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RegisterLevelBonusConfirmed(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
                        ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response,
                        ::google::protobuf::Closure* done) override;

private:

    void EnhanceImpl(
        UserContext &user,
        const ::apb::api::costume::EnhanceRequest* request,
        ::apb::api::costume::EnhanceResponse* response);

    void LimitBreakImpl(
        UserContext &user,
        const ::apb::api::costume::LimitBreakRequest* request,
        ::apb::api::costume::LimitBreakResponse* response);

    void EnhanceActiveSkillImpl(
        UserContext &user,
        const ::apb::api::costume::EnhanceActiveSkillRequest* request,
        ::apb::api::costume::EnhanceActiveSkillResponse* response);

    void RegisterLevelBonusConfirmedImpl(
        UserContext &user,
        const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
        ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response);
};

#endif




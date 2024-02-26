#ifndef SERVICE_IMPLEMENTATIONS_COSTUME_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COSTUME_SERVICE_H

#include <service/CostumeService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class CostumeService final : public apb::api::costume::CostumeService::Service, public CommonService {
public:
    explicit CostumeService(Database &db);
    ~CostumeService();

    CostumeService(const CostumeService &other) = delete;
    CostumeService &operator =(const CostumeService &other) = delete;

    ::grpc::Status Enhance(
        ::grpc::ServerContext* context,
        const ::apb::api::costume::EnhanceRequest* request,
        ::apb::api::costume::EnhanceResponse* response) override;

    ::grpc::Status EnhanceActiveSkill(
        ::grpc::ServerContext* context,
        const ::apb::api::costume::EnhanceActiveSkillRequest* request,
        ::apb::api::costume::EnhanceActiveSkillResponse* response) override;

    ::grpc::Status RegisterLevelBonusConfirmed(
        ::grpc::ServerContext* context,
        const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
        ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response) override;

private:

    void EnhanceImpl(
        UserContext &user,
        const ::apb::api::costume::EnhanceRequest* request,
        ::apb::api::costume::EnhanceResponse* response);

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




#ifndef SERVICE_IMPLEMENTATIONS_WEAPON_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_WEAPON_SERVICE_H

#include <service/WeaponService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class WeaponService final : public apb::api::weapon::WeaponService::Service, public CommonService {
public:
    explicit WeaponService(Database &db);
    ~WeaponService();

    WeaponService(const WeaponService &other) = delete;
    WeaponService &operator =(const WeaponService &other) = delete;

    ::grpc::Status Protect(
        ::grpc::ServerContext* context, const ::apb::api::weapon::ProtectRequest* request, ::apb::api::weapon::ProtectResponse* response) override;

    ::grpc::Status Unprotect(
        ::grpc::ServerContext* context, const ::apb::api::weapon::UnprotectRequest* request, ::apb::api::weapon::UnprotectResponse* response) override;

    ::grpc::Status EnhanceByMaterial(
        ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceByMaterialRequest* request,
        ::apb::api::weapon::EnhanceByMaterialResponse* response) override;

    ::grpc::Status EnhanceSkill(
        ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceSkillRequest* request, ::apb::api::weapon::EnhanceSkillResponse* response) override;

    ::grpc::Status EnhanceAbility(
        ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceAbilityRequest* request, ::apb::api::weapon::EnhanceAbilityResponse* response) override;

private:
    void ProtectImpl(
        UserContext &user, const ::apb::api::weapon::ProtectRequest* request, ::apb::api::weapon::ProtectResponse* response);

    void UnprotectImpl(
        UserContext &user, const ::apb::api::weapon::UnprotectRequest* request, ::apb::api::weapon::UnprotectResponse* response);

    void EnhanceByMaterialImpl(
        UserContext &user, const ::apb::api::weapon::EnhanceByMaterialRequest* request,
        ::apb::api::weapon::EnhanceByMaterialResponse* response);

    void EnhanceSkillImpl(
        UserContext &user, const ::apb::api::weapon::EnhanceSkillRequest* request, ::apb::api::weapon::EnhanceSkillResponse* response);

    void EnhanceAbilityImpl(
        UserContext &user, const ::apb::api::weapon::EnhanceAbilityRequest* request, ::apb::api::weapon::EnhanceAbilityResponse* response);
};

#endif


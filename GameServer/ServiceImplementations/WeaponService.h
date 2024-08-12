#ifndef SERVICE_IMPLEMENTATIONS_WEAPON_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_WEAPON_SERVICE_H

#include <service/WeaponService.pb.h>

#include <ServiceImplementations/CommonService.h>

class WeaponService final : public apb::api::weapon::WeaponService, public CommonService {
public:
    WeaponService(Database &db, const GameServerConfiguration &config);
    ~WeaponService();

    WeaponService(const WeaponService &other) = delete;
    WeaponService &operator =(const WeaponService &other) = delete;

    void Protect(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::ProtectRequest* request,
                        ::apb::api::weapon::ProtectResponse* response,
                        ::google::protobuf::Closure* done) override;

    void Unprotect(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::UnprotectRequest* request,
                        ::apb::api::weapon::UnprotectResponse* response,
                        ::google::protobuf::Closure* done) override;

    void EnhanceByMaterial(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceByMaterialRequest* request,
                            ::apb::api::weapon::EnhanceByMaterialResponse* response,
                            ::google::protobuf::Closure* done) override;

    void EnhanceSkill(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceSkillRequest* request,
                            ::apb::api::weapon::EnhanceSkillResponse* response,
                            ::google::protobuf::Closure* done) override;

    void EnhanceAbility(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceAbilityRequest* request,
                            ::apb::api::weapon::EnhanceAbilityResponse* response,
                            ::google::protobuf::Closure* done) override;

    void LimitBreakByMaterial(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::LimitBreakByMaterialRequest* request,
                            ::apb::api::weapon::LimitBreakByMaterialResponse* response,
                            ::google::protobuf::Closure* done) override;

    void Evolve(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::EvolveRequest* request,
                        ::apb::api::weapon::EvolveResponse* response,
                        ::google::protobuf::Closure* done) override;

    void Awaken(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::AwakenRequest* request,
                        ::apb::api::weapon::AwakenResponse* response,
                        ::google::protobuf::Closure* done) override;
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

    void LimitBreakByMaterialImpl(
        UserContext &user, const ::apb::api::weapon::LimitBreakByMaterialRequest* request, ::apb::api::weapon::LimitBreakByMaterialResponse* response);

    void EvolveImpl(
        UserContext &user, const ::apb::api::weapon::EvolveRequest* request, ::apb::api::weapon::EvolveResponse* response);

    void AwakenImpl(
        UserContext &user, const ::apb::api::weapon::AwakenRequest* request, ::apb::api::weapon::AwakenResponse* response);
};

#endif


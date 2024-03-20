#include <ServiceImplementations/WeaponService.h>

WeaponService::WeaponService(Database &db) : CommonService(db) {

}

WeaponService::~WeaponService() = default;

void WeaponService::Protect(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::ProtectRequest* request,
                        ::apb::api::weapon::ProtectResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::Protect", controller, request, response, done, &WeaponService::ProtectImpl);
}

void WeaponService::Unprotect(::google::protobuf::RpcController* controller,
                        const ::apb::api::weapon::UnprotectRequest* request,
                        ::apb::api::weapon::UnprotectResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::Unprotect", controller, request, response, done, &WeaponService::UnprotectImpl);
}

void WeaponService::EnhanceByMaterial(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceByMaterialRequest* request,
                            ::apb::api::weapon::EnhanceByMaterialResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::EnhanceByMaterial", controller, request, response, done,
                           &WeaponService::EnhanceByMaterialImpl);
}

void WeaponService::EnhanceSkill(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceSkillRequest* request,
                            ::apb::api::weapon::EnhanceSkillResponse* response,
                            ::google::protobuf::Closure* done)  {

    return inChangesetCall("WeaponService::EnhanceSkill", controller, request, response, done, &WeaponService::EnhanceSkillImpl);
}

void WeaponService::EnhanceAbility(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EnhanceAbilityRequest* request,
                            ::apb::api::weapon::EnhanceAbilityResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::EnhanceAbility", controller, request, response, done, &WeaponService::EnhanceAbilityImpl);
}

void WeaponService::LimitBreakByMaterial(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::LimitBreakByMaterialRequest* request,
                            ::apb::api::weapon::LimitBreakByMaterialResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::LimitBreakByMaterial", controller, request, response,
                           done, &WeaponService::LimitBreakByMaterialImpl);
}

void WeaponService::Evolve(::google::protobuf::RpcController* controller,
                            const ::apb::api::weapon::EvolveRequest* request,
                            ::apb::api::weapon::EvolveResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("WeaponService::Evolve", controller, request, response,
                           done, &WeaponService::EvolveImpl);
}

void WeaponService::ProtectImpl(UserContext &user, const ::apb::api::weapon::ProtectRequest* request, ::apb::api::weapon::ProtectResponse* response) {

    for(const auto &uuid: request->user_weapon_uuid()) {
        user.setWeaponProtected(uuid, true);
    }
}

void WeaponService::UnprotectImpl(
        UserContext &user, const ::apb::api::weapon::UnprotectRequest* request, ::apb::api::weapon::UnprotectResponse* response) {

    for(const auto &uuid: request->user_weapon_uuid()) {
        user.setWeaponProtected(uuid, false);
    }
}

void WeaponService::EnhanceByMaterialImpl(
    UserContext &user, const ::apb::api::weapon::EnhanceByMaterialRequest* request,
    ::apb::api::weapon::EnhanceByMaterialResponse* response) {

    int32_t weaponTotalEnhancementCost = 0;
    int32_t effectValue = 0;

    for(const auto &materialItemIdAndCount: request->materials()) {
        int32_t costForThisMaterial;
        int32_t rarity;

        user.queryWeaponEnhancementCost(request->user_weapon_uuid(), materialItemIdAndCount.second,
                                         costForThisMaterial);

        effectValue = user.consumeEnhancementMaterialAndCalculateTotalEffectValue(
            materialItemIdAndCount.first,
            materialItemIdAndCount.second,
            MaterialType::WEAPON_ENHANCEMENT);

        weaponTotalEnhancementCost += costForThisMaterial;
    }

    printf("WeaponService::EnhanceByMaterialImpl: total enhancement value: %d, total enhancement cost: %d\n", effectValue, weaponTotalEnhancementCost);

    user.consumeConsumableItem(user.consumableItemIdForGold(), weaponTotalEnhancementCost);

    user.giveUserWeaponExperience(request->user_weapon_uuid(), 0, effectValue);
}

void WeaponService::EnhanceSkillImpl(
    UserContext &user, const ::apb::api::weapon::EnhanceSkillRequest* request,
    ::apb::api::weapon::EnhanceSkillResponse* response) {

    if(request->add_level_count() < 0)
        throw std::logic_error("bad level count");

    for(int32_t levelToAdd = 0; levelToAdd < request->add_level_count(); levelToAdd++) {
        user.enhanceWeaponSkill(request->user_weapon_uuid(), request->skill_id());
    }
}

void WeaponService::EnhanceAbilityImpl(
    UserContext &user, const ::apb::api::weapon::EnhanceAbilityRequest* request,
    ::apb::api::weapon::EnhanceAbilityResponse* response) {

    if(request->add_level_count() < 0)
        throw std::logic_error("bad level count");

    for(int32_t levelToAdd = 0; levelToAdd < request->add_level_count(); levelToAdd++) {
        user.enhanceWeaponAbility(request->user_weapon_uuid(), request->ability_id());
    }
}

void WeaponService::LimitBreakByMaterialImpl(
    UserContext &user, const ::apb::api::weapon::LimitBreakByMaterialRequest* request, ::apb::api::weapon::LimitBreakByMaterialResponse* response) {

    user.weaponLimitBreak(
        request->user_weapon_uuid(),
        request->materials());
}


void WeaponService::EvolveImpl(
    UserContext &user, const ::apb::api::weapon::EvolveRequest* request, ::apb::api::weapon::EvolveResponse* response) {

    user.weaponEvolve(request->user_weapon_uuid());
}

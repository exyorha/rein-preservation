#include <ServiceImplementations/WeaponService.h>

WeaponService::WeaponService(Database &db) : CommonService(db) {

}

WeaponService::~WeaponService() = default;

::grpc::Status WeaponService::Protect(
    ::grpc::ServerContext* context, const ::apb::api::weapon::ProtectRequest* request, ::apb::api::weapon::ProtectResponse* response) {

    return inChangesetCall("WeaponService::Protect", context, request, response, &WeaponService::ProtectImpl);
}

::grpc::Status WeaponService::Unprotect(
    ::grpc::ServerContext* context, const ::apb::api::weapon::UnprotectRequest* request, ::apb::api::weapon::UnprotectResponse* response) {

    return inChangesetCall("WeaponService::Unprotect", context, request, response, &WeaponService::UnprotectImpl);
}

::grpc::Status WeaponService::EnhanceByMaterial(
    ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceByMaterialRequest* request,
    ::apb::api::weapon::EnhanceByMaterialResponse* response) {

    return inChangesetCall("WeaponService::EnhanceByMaterial", context, request, response, &WeaponService::EnhanceByMaterialImpl);
}

::grpc::Status WeaponService::EnhanceSkill(
    ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceSkillRequest* request,
    ::apb::api::weapon::EnhanceSkillResponse* response) {

    return inChangesetCall("WeaponService::EnhanceSkill", context, request, response, &WeaponService::EnhanceSkillImpl);
}

::grpc::Status WeaponService::EnhanceAbility(
    ::grpc::ServerContext* context, const ::apb::api::weapon::EnhanceAbilityRequest* request,
    ::apb::api::weapon::EnhanceAbilityResponse* response) {

    return inChangesetCall("WeaponService::EnhanceAbility", context, request, response, &WeaponService::EnhanceAbilityImpl);
}

::grpc::Status WeaponService::LimitBreakByMaterial(
    ::grpc::ServerContext* context, const ::apb::api::weapon::LimitBreakByMaterialRequest* request,
    ::apb::api::weapon::LimitBreakByMaterialResponse* response) {

    return inChangesetCall("WeaponService::LimitBreakByMaterial", context, request, response, &WeaponService::LimitBreakByMaterialImpl);
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

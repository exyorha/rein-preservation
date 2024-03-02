#include "ServiceImplementations/CostumeService.h"
#include "DataModel/DatabaseEnums.h"

CostumeService::CostumeService(Database &db) : CommonService(db) {

}

CostumeService::~CostumeService() = default;

void CostumeService::Enhance(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::EnhanceRequest* request,
                        ::apb::api::costume::EnhanceResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inChangesetCall("CostumeService::Enhance", controller, request, response, done, &CostumeService::EnhanceImpl);
}

void CostumeService::LimitBreak(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::LimitBreakRequest* request,
                        ::apb::api::costume::LimitBreakResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CostumeService::LimitBreak", controller, request, response, done, &CostumeService::LimitBreakImpl);
}

void CostumeService::EnhanceActiveSkill(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::EnhanceActiveSkillRequest* request,
                        ::apb::api::costume::EnhanceActiveSkillResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CostumeService::EnhanceActiveSkill", controller, request, response, done,
                           &CostumeService::EnhanceActiveSkillImpl);
}

void CostumeService::RegisterLevelBonusConfirmed(::google::protobuf::RpcController* controller,
                        const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
                        ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inChangesetCall("CostumeService::RegisterLevelBonusConfirmed", controller, request, response, done, &CostumeService::RegisterLevelBonusConfirmedImpl);
}

void CostumeService::EnhanceImpl(
    UserContext &user,
    const ::apb::api::costume::EnhanceRequest* request,
    ::apb::api::costume::EnhanceResponse* response) {

    int32_t costumeTotalEnhancementCost = 0;
    int32_t effectValue = 0;

    for(const auto &materialItemIdAndCount: request->materials()) {
        int32_t costForThisMaterial;
        int32_t rarity;

        user.queryCostumeEnhancementCost(request->user_costume_uuid(), materialItemIdAndCount.second,
                                         costForThisMaterial);

        effectValue = user.consumeEnhancementMaterialAndCalculateTotalEffectValue(
            materialItemIdAndCount.first,
            materialItemIdAndCount.second,
            MaterialType::COSTUME_ENHANCEMENT);

        costumeTotalEnhancementCost += costForThisMaterial;
    }

    printf("CostumeService::EnhanceImpl: total enhancement value: %d, total enhancement cost: %d\n", effectValue, costumeTotalEnhancementCost);

    user.consumeConsumableItem(user.consumableItemIdForGold(), costumeTotalEnhancementCost);

    user.giveUserCostumeExperience(request->user_costume_uuid(), 0, effectValue);
}

void CostumeService::LimitBreakImpl(
    UserContext &user,
    const ::apb::api::costume::LimitBreakRequest* request,
    ::apb::api::costume::LimitBreakResponse* response) {

    user.costumeLimitBreak(
        request->user_costume_uuid(),
        request->materials());
}

void CostumeService::EnhanceActiveSkillImpl(
    UserContext &user,
    const ::apb::api::costume::EnhanceActiveSkillRequest* request,
    ::apb::api::costume::EnhanceActiveSkillResponse* response) {

    if(request->add_level_count() < 0)
        throw std::logic_error("bad level count");

    for(int32_t levelToAdd = 0; levelToAdd < request->add_level_count(); levelToAdd++) {
        user.enhanceCostumeActiveSkill(request->user_costume_uuid());
    }
}

void CostumeService::RegisterLevelBonusConfirmedImpl(
    UserContext &user,
    const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
    ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response) {

    user.registerCostumeLevelBonusConfirmed(request->costume_id(), request->level());
}

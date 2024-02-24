#include "ServiceImplementations/CostumeService.h"
#include "DataModel/DatabaseEnums.h"

CostumeService::CostumeService(Database &db) : CommonService(db) {

}

CostumeService::~CostumeService() = default;

::grpc::Status CostumeService::Enhance(
    ::grpc::ServerContext* context,
    const ::apb::api::costume::EnhanceRequest* request,
    ::apb::api::costume::EnhanceResponse* response) {

    return inChangesetCall("CostumeService::Enhance", context, request, response, &CostumeService::EnhanceImpl);
}

::grpc::Status CostumeService::RegisterLevelBonusConfirmed(
    ::grpc::ServerContext* context,
    const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
    ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response) {

    return inChangesetCall("CostumeService::RegisterLevelBonusConfirmed", context, request, response, &CostumeService::RegisterLevelBonusConfirmedImpl);
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

void CostumeService::RegisterLevelBonusConfirmedImpl(
    UserContext &user,
    const ::apb::api::costume::RegisterLevelBonusConfirmedRequest* request,
    ::apb::api::costume::RegisterLevelBonusConfirmedResponse* response) {

    user.registerCostumeLevelBonusConfirmed(request->costume_id(), request->level());
}

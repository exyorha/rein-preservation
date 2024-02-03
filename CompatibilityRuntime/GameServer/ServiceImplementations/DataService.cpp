#include "DataService.h"

DataService::DataService() = default;

DataService::~DataService() = default;

::grpc::Status DataService::GetLatestMasterDataVersion(::grpc::ServerContext* context,
                                                       const ::google::protobuf::Empty* request,
                                                       ::apb::api::data::MasterDataGetLatestVersionResponse* response) {

    printf("DataService::GetLatestMasterDataVersion\n");

    response->set_latest_master_data_version("prd-us/20240130174048");

    return grpc::Status::OK;
}

::grpc::Status DataService::GetUserDataName(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                               ::apb::api::data::UserDataGetNameResponse* response) {

    printf("DataService::GetUserDataName\n");

    for(const auto &name: getUserDataName()) {
        response->add_table_name(name);
    }

    for(const auto &name: getUserDataName2()) {
        response->add_table_name(name);
    }

    return grpc::Status::OK;
}


::grpc::Status DataService::GetUserDataNameV2(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                 ::apb::api::data::UserDataGetNameResponseV2* response) {

    printf("DataService::GetUserDataNameV2\n");

    auto list = response->add_table_name_list();

    for(const auto &name: getUserDataName()) {
        list->add_table_name(name);
    }

    list = response->add_table_name_list();

    for(const auto &name: getUserDataName2()) {
        list->add_table_name(name);
    }

    return grpc::Status::OK;
}

std::vector<std::string> DataService::getUserDataName() const {
    return {
        "IUserLoginBonus",
        "IUserSideStoryQuestSceneProgressStatus",
        "IUserLimitedOpen",
        "IUserBigHuntMaxScore",
        "IUserDeck",
        "IUserMainQuestFlowStatus",
        "IUserCompanion",
        "IUserCharacterRebirth",
        "IUserWeaponSkill",
        "IUserWeapon",
        "IUserQuestSceneChoice",
        "IUserWebviewPanelMission",
        "IUserStatus",
        "IUserDeckLimitContentRestricted",
        "IUserTutorialProgress",
        "IUserEventQuestGuerrillaFreeOpen",
        "IUserCostumeLotteryEffect",
        "IUserExploreScore",
        "IUserDeckSubWeaponGroup",
        "IUserExplore",
        "IUserEventQuestTowerAccumulationReward",
        "IUserShopItem",
        "IUserDokan",
        "IUserOmikuji",
        "IUserWeaponAbility",
        "IUserConsumableItem",
        "IUserMainQuestReplayFlowStatus",
        "IUserWeaponAwaken",
        "IUserParts",
        "IUserEventQuestDailyGroupCompleteReward",
        "IUserMainQuestProgressStatus",
        "IUserCostumeLotteryEffectAbility",
        "IUserAutoSaleSettingDetail",
        "IUserGimmickSequence",
        "IUserQuestLimitContentStatus",
        "IUserGem",
        "IUserCostumeLevelBonusReleaseStatus",
        "IUserTripleDeck",
        "IUserComebackCampaign",
        "IUserCharacterBoardCompleteReward",
        "IUserCharacterCostumeLevelBonus",
        "IUserCostumeLotteryEffectStatusUp",
        "IUserCostumeAwakenStatusUp",
        "IUserContentsStory",
        "IUserEventQuestProgressStatus",
        "IUserNaviCutIn",
        "IUserQuestSceneChoiceHistory",
        "IUserMissionCompletionProgress",
        "IUserMovie",
        "IUserGimmickUnlock",
        "IUserBigHuntWeeklyStatus",
        "IUserEventQuestLabyrinthSeason",
        "IUserGimmickOrnamentProgress",
        "IUserSideStoryQuest",
        "IUserLogin",
        "IUserBigHuntStatus",
        "IUserDeckPartsGroup",
        "IUserSetting",
        "IUserPartsStatusSub",
        "IUserPremiumItem",
        "IUserMainQuestMainFlowStatus",
        "IUserCharacterViewerField",
        "IUser",
        "IUserCharacterBoard",
        "IUserBeginnerCampaign",
        "IUserBigHuntProgressStatus",
        "IUserCostume",
        "IUserPartsPreset",
        "IUserCharacterBoardStatusUp",
        "IUserFacebook",
        "IUserWeaponNote",
        "IUserMissionPassPoint",
        "IUserCageOrnamentReward",
        "IUserPortalCageStatus",
        "IUserCostumeActiveSkill",
        "IUserWeaponStory",
        "IUserPartsGroupNote",
        "IUserBigHuntScheduleMaxScore",
        "IUserQuestAutoOrbit",
        "IUserImportantItem",
        "IUserDeckCharacter",
        "IUserPossessionAutoConvert",
        "IUserPartsPresetTag",
        "IUserMaterial",
        "IUserMainQuestSeasonRoute",
        "IUserDeckCharacterDressupCostume",
        "IUserDeckTypeNote",
        "IUserCharacterBoardAbility",
        "IUserPvpDefenseDeck",
        "IUserExtraQuestProgressStatus",
        "IUserBigHuntWeeklyMaxScore",
        "IUserEventQuestLabyrinthStage",
        "IUserGimmick",
        "IUserShopReplaceableLineup",
        "IUserApple",
        "IUserThought",
        "IUserProfile",
        "IUserShopReplaceable",
        "IUserQuestReplayFlowRewardGroup",
        "IUserCostumeLotteryEffectPending",
        "IUserCharacter",
        "IUserPvpStatus",
        "IUserPvpWeeklyResult"
    };
}

std::vector<std::string> DataService::getUserDataName2() const {
    return {
        /*
        * List2
        */
        "IUserQuest",
        "IUserQuestMission",
        "IUserMission",
    };
}

::grpc::Status DataService::GetUserData(::grpc::ServerContext* context,
                                        const ::apb::api::data::UserDataGetRequest* request,
                                        ::apb::api::data::UserDataGetResponse* response) {

    printf("DataService::GetUserData: %s\n", request->DebugString().c_str());

    for(const auto &tableName: request->table_name()) {
        std::string_view content;

        if(tableName == "IUserProfile") {
            content = R"json([{"userId":1,"name":"Ash","nameUpdateDatetime":28800000,"message":"","messageUpdateDatetime":28800000,"favoriteCostumeId":0,"favoriteCostumeIdUpdateDatetime":28800000,"latestVersion":1}])json";
        } else if(tableName == "IUser") {
            content = R"json([{"userId":1,"playerId":259591998318,"osType":2,"platformType":2,"userRestrictionType":1,"registerDatetime":1706304564000,"gameStartDatetime":28800000,"latestVersion":1}])json";
        } else if(tableName == "IUserLogin") {
            content = R"json([{"userId":1,"totalLoginCount":0,"continualLoginCount":0,"maxContinualLoginCount":0,"lastLoginDatetime":28800000,"lastComebackLoginDatetime":28800000,"latestVersion":1}]")json";
        } else if(tableName == "IUserStatus") {
            content = R"json([{"userId":1,"level":1,"exp":0,"staminaMilliValue":50000,"staminaUpdateDatetime":1706304564000,"latestVersion":1}])json";
        } else if(tableName == "IUserGem") {
            content = R"json([{"userId":1,"paidGem":0,"freeGem":0}])json";
        } else if(tableName == "IUserMainQuestMainFlowStatus") {
            content = R"json([{"userId":1,"currentMainQuestRouteId":1,"currentQuestSceneId":0,"headQuestSceneId":0,"isReachedLastQuestScene":false,"latestVersion":1}])json";
        } else {
            content = "[]";
        }

        response->mutable_user_data_json()->emplace(tableName, content);
    }

    return grpc::Status::OK;
}


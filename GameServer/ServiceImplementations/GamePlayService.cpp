#include "GamePlayService.h"
#include "DataModel/DatabaseEnums.h"

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/DatabaseEnums.h>

GamePlayService::GamePlayService(Database &db) : CommonService(db) {

}

GamePlayService::~GamePlayService() = default;


::grpc::Status GamePlayService::CheckBeforeGamePlay(::grpc::ServerContext* context,
                                    const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                    ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) {

    return inChangesetCall("GamePlayService::CheckBeforeGamePlay", context, request, response, &GamePlayService::CheckBeforeGamePlayImpl);

}

void GamePlayService::CheckBeforeGamePlayImpl(int64_t userId,
                                    const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                    ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) {

    // TODO: IUserLogin is updated here

    // TODO: IUserMission is initially populated here?
    // TODO: IUserWeaponNote is initially populated here?
    // TODO: IUserWeaponStory is initially populated here?

    buildDefaultDeckIfNoneExists(userId);

    for(int32_t defaultWeapon: { 100001, 100011, 100021 }) {
        bool hasWeapon = false;
        auto hasWeaponQuery = db().prepare("SELECT 1 FROM i_user_weapon WHERE user_id = ? AND weapon_id = ? LIMIT 1");
        hasWeaponQuery->bind(1, userId);
        hasWeaponQuery->bind(2, defaultWeapon);
        if(hasWeaponQuery->step()) {
            hasWeapon = true;
        }

        if(!hasWeapon) {
            givePossession(userId, static_cast<int32_t>(PossessionType::WEAPON), defaultWeapon, 1);
        }
    }
}


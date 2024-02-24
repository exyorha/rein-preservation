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

void GamePlayService::CheckBeforeGamePlayImpl(UserContext &user,
                                    const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                    ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) {

    // TODO: IUserLogin is updated here

    // TODO: IUserMission is initially populated here?
    // TODO: IUserWeaponNote is initially populated here?
    // TODO: IUserWeaponStory is initially populated here?

    user.buildDefaultDeckIfNoneExists();

    /*
     * The live server does that for new characters and it's necessary for the mom menu tutorial.
     */
    for(int32_t defaultWeapon: { 100001, 100011, 100021 }) {
        if(!user.hasWeaponWithId(defaultWeapon)) {
            user.givePossession(static_cast<int32_t>(PossessionType::WEAPON), defaultWeapon, 1);
        }
    }

    /*
     * This is needed for the companion tutorial to not get wedged.
     */

    if(!user.hasAnyCompanions()) {
        /*
         * This should depend on the tutorial choices and such, but we
         * currently lack the needed stuff, so we just give out *something*
         * for the game to not get stuck.
         */
        user.givePossession(static_cast<int32_t>(PossessionType::COMPANION), 1, 1);
    }

    user.updateUserUnlocks();
}


#include "GamePlayService.h"
#include "DataModel/DatabaseEnums.h"

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/DatabaseEnums.h>

GamePlayService::GamePlayService(Database &db) : CommonService(db) {

}

GamePlayService::~GamePlayService() = default;

void GamePlayService::CheckBeforeGamePlay(::google::protobuf::RpcController* controller,
                            const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                            ::apb::api::gameplay::CheckBeforeGamePlayResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("GamePlayService::CheckBeforeGamePlay", controller, request, response, done,
                           &GamePlayService::CheckBeforeGamePlayImpl);

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

    user.beforeGamePlay();
}


#include "GamePlayService.h"

GamePlayService::GamePlayService(Database &db) : CommonService(db) {

}

GamePlayService::~GamePlayService() = default;


::grpc::Status GamePlayService::CheckBeforeGamePlay(::grpc::ServerContext* context,
                                    const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                    ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) {

    // TODO: IUserLogin is updated here

    // TODO: IUserMission is initially populated here?
    // TODO: IUserWeaponNote is initially populated here?
    // TODO: IUserWeaponStory is initially populated here?

    printf("GamePlayService::CheckBeforeGamePlay: %s\n", request->DebugString().c_str());

    return grpc::Status::OK;
}

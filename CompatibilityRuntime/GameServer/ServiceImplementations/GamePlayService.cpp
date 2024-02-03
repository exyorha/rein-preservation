#include "GamePlayService.h"

GamePlayService::GamePlayService() = default;

GamePlayService::~GamePlayService() = default;


::grpc::Status GamePlayService::CheckBeforeGamePlay(::grpc::ServerContext* context,
                                    const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                    ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) {

    printf("GamePlayService::CheckBeforeGamePlay: %s\n", request->DebugString().c_str());

    return grpc::Status::OK;
}

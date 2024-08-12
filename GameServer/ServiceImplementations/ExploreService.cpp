#include <ServiceImplementations/ExploreService.h>

ExploreService::ExploreService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

ExploreService::~ExploreService() = default;

void ExploreService::StartExplore(::google::protobuf::RpcController* controller,
                            const ::apb::api::explore::StartExploreRequest* request,
                            ::apb::api::explore::StartExploreResponse* response,
                            ::google::protobuf::Closure* done) {
    return inChangesetCall("ExploreService::StartExplore", controller, request, response, done,
                           &ExploreService::StartExploreImpl);
}

void ExploreService::FinishExplore(::google::protobuf::RpcController* controller,
                            const ::apb::api::explore::FinishExploreRequest* request,
                            ::apb::api::explore::FinishExploreResponse* response,
                            ::google::protobuf::Closure* done) {
    return inChangesetCall("ExploreService::FinishExplore", controller, request, response, done,
                           &ExploreService::FinishExploreImpl);
}

void ExploreService::RetireExplore(::google::protobuf::RpcController* controller,
                            const ::apb::api::explore::RetireExploreRequest* request,
                            ::apb::api::explore::RetireExploreResponse* response,
                            ::google::protobuf::Closure* done) {
    return inChangesetCall("ExploreService::RetireExplore", controller, request, response, done,
                           &ExploreService::RetireExploreImpl);
}

void ExploreService::StartExploreImpl(UserContext &user,
                        const ::apb::api::explore::StartExploreRequest* request,
                        ::apb::api::explore::StartExploreResponse* response) {

    user.startExplore(
        request->explore_id(),
        request->use_consumable_item_id());
}

void ExploreService::FinishExploreImpl(UserContext &user,
                        const ::apb::api::explore::FinishExploreRequest* request,
                        ::apb::api::explore::FinishExploreResponse* response) {

    int32_t assetGradeIconId;

    user.finishExplore(request->explore_id(), request->score(), assetGradeIconId);

    response->set_asset_grade_icon_id(assetGradeIconId);
}

void ExploreService::RetireExploreImpl(UserContext &user,
                        const ::apb::api::explore::RetireExploreRequest* request,
                    ::apb::api::explore::RetireExploreResponse* response) {

    user.retireExplore(request->explore_id());
}

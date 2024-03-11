#include <ServiceImplementations/CageOrnamentService.h>

CageOrnamentService::CageOrnamentService(Database &db) : CommonService(db) {

}

CageOrnamentService::~CageOrnamentService() = default;

void CageOrnamentService::ReceiveReward(::google::protobuf::RpcController* controller,
                        const ::apb::api::cageornament::ReceiveRewardRequest* request,
                        ::apb::api::cageornament::ReceiveRewardResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CageOrnamentService::ReceiveReward", controller, request, response, done,
                           &CageOrnamentService::ReceiveRewardImpl);
}


void CageOrnamentService::ReceiveRewardImpl(
    UserContext &user,
    const ::apb::api::cageornament::ReceiveRewardRequest* request,
    ::apb::api::cageornament::ReceiveRewardResponse* response) {

    user.activateCageOrnament(request->cage_ornament_id(), response->mutable_cage_ornament_reward());
}

void CageOrnamentService::RecordAccess(::google::protobuf::RpcController* controller,
                        const ::apb::api::cageornament::RecordAccessRequest* request,
                        ::apb::api::cageornament::RecordAccessResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CageOrnamentService::RecordAccess", controller, request, response, done,
                           &CageOrnamentService::RecordAccessImpl);
}

void CageOrnamentService::RecordAccessImpl(
    UserContext &user,
    const ::apb::api::cageornament::RecordAccessRequest* request,
    ::apb::api::cageornament::RecordAccessResponse* response) {

    user.activateCageOrnament(request->cage_ornament_id());
}

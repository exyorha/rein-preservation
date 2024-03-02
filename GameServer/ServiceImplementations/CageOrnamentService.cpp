#include <ServiceImplementations/CageOrnamentService.h>

CageOrnamentService::CageOrnamentService(Database &db) : CommonService(db) {

}

CageOrnamentService::~CageOrnamentService() = default;


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

    user.recordCageOrnamentAccess(request->cage_ornament_id());
}

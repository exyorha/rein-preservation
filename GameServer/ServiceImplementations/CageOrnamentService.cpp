#include <ServiceImplementations/CageOrnamentService.h>

CageOrnamentService::CageOrnamentService(Database &db) : CommonService(db) {

}

CageOrnamentService::~CageOrnamentService() = default;


::grpc::Status CageOrnamentService::RecordAccess(
    ::grpc::ServerContext* context, const ::apb::api::cageornament::RecordAccessRequest* request,
    ::apb::api::cageornament::RecordAccessResponse* response) {

    return inChangesetCall("CageOrnamentService::RecordAccess", context, request, response, &CageOrnamentService::RecordAccessImpl);
}

void CageOrnamentService::RecordAccessImpl(
    UserContext &user,
    const ::apb::api::cageornament::RecordAccessRequest* request,
    ::apb::api::cageornament::RecordAccessResponse* response) {

    user.recordCageOrnamentAccess(request->cage_ornament_id());
}

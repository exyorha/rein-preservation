#include <ServiceImplementations/NaviCutInService.h>

#include <DataModel/Sqlite/Statement.h>

NaviCutInService::NaviCutInService(Database &db) :CommonService(db) {

}

NaviCutInService::~NaviCutInService() = default;


::grpc::Status NaviCutInService::RegisterPlayed(::grpc::ServerContext* context, const ::apb::api::navicutin::RegisterPlayedRequest* request,
                                        ::apb::api::navicutin::RegisterPlayedResponse* response) {

    return inChangesetCall("NaviCutInService::RegisterPlayed", context, request, response, &NaviCutInService::RegisterPlayedImpl);
}

void NaviCutInService::RegisterPlayedImpl(
    UserContext &user,
    const ::apb::api::navicutin::RegisterPlayedRequest* request,
    ::apb::api::navicutin::RegisterPlayedResponse* response) {

    user.registerNaviCutInPlayed(request->navi_cut_id());
}

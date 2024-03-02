#include <ServiceImplementations/NaviCutInService.h>

#include <DataModel/Sqlite/Statement.h>

NaviCutInService::NaviCutInService(Database &db) :CommonService(db) {

}

NaviCutInService::~NaviCutInService() = default;


void NaviCutInService::RegisterPlayed(::google::protobuf::RpcController* controller,
                        const ::apb::api::navicutin::RegisterPlayedRequest* request,
                        ::apb::api::navicutin::RegisterPlayedResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("NaviCutInService::RegisterPlayed", controller, request, response, done,
                           &NaviCutInService::RegisterPlayedImpl);
}

void NaviCutInService::RegisterPlayedImpl(
    UserContext &user,
    const ::apb::api::navicutin::RegisterPlayedRequest* request,
    ::apb::api::navicutin::RegisterPlayedResponse* response) {

    user.registerNaviCutInPlayed(request->navi_cut_id());
}

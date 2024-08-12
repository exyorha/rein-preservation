#include <ServiceImplementations/PortalCageService.h>

PortalCageService::PortalCageService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

PortalCageService::~PortalCageService() = default;


void PortalCageService::UpdatePortalCageSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
                            ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response,
                            ::google::protobuf::Closure* done)  {

    return inChangesetCall("PortalCageService::UpdatePortalCageSceneProgress", controller, request, response, done,
                           &PortalCageService::UpdatePortalCageSceneProgressImpl);

}

void PortalCageService::UpdatePortalCageSceneProgressImpl(
    UserContext &user,
    const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
    ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response) {

    user.updatePortalCageSceneProgress(request->portal_cage_scene_id());
}

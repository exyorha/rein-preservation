#include <ServiceImplementations/PortalCageService.h>

PortalCageService::PortalCageService(Database &db) :CommonService(db) {

}

PortalCageService::~PortalCageService() = default;


::grpc::Status PortalCageService::UpdatePortalCageSceneProgress(
    ::grpc::ServerContext* context,
    const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
    ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response) {

    return inChangesetCall("PortalCageService::UpdatePortalCageSceneProgress", context, request, response,  &PortalCageService::UpdatePortalCageSceneProgressImpl);

}

void PortalCageService::UpdatePortalCageSceneProgressImpl(
    UserContext &user,
    const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
    ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response) {

    // TODO
}

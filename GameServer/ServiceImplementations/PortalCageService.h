#ifndef SERVICE_IMPLEMENTATIONS_PORTAL_CAGE_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_PORTAL_CAGE_SERVICE_H

#include <service/PortalCageService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class PortalCageService final : public apb::api::portalcage::PortalCageService::Service, public CommonService {
public:
    explicit PortalCageService(Database &db);
    ~PortalCageService();

    PortalCageService(const PortalCageService &other) = delete;
    PortalCageService &operator =(const PortalCageService &other) = delete;

    ::grpc::Status UpdatePortalCageSceneProgress(
        ::grpc::ServerContext* context,
        const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
        ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response) override;

private:

    void UpdatePortalCageSceneProgressImpl(
        int64_t userId,
        const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
        ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response);
};

#endif



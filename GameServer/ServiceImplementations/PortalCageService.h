#ifndef SERVICE_IMPLEMENTATIONS_PORTAL_CAGE_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_PORTAL_CAGE_SERVICE_H

#include <service/PortalCageService.pb.h>

#include <ServiceImplementations/CommonService.h>

class PortalCageService final : public apb::api::portalcage::PortalCageService, public CommonService {
public:
    explicit PortalCageService(Database &db);
    ~PortalCageService();

    PortalCageService(const PortalCageService &other) = delete;
    PortalCageService &operator =(const PortalCageService &other) = delete;

    void UpdatePortalCageSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
                            ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response,
                            ::google::protobuf::Closure* done) override;

private:

    void UpdatePortalCageSceneProgressImpl(
        UserContext &user,
        const ::apb::api::portalcage::UpdatePortalCageSceneProgressRequest* request,
        ::apb::api::portalcage::UpdatePortalCageSceneProgressResponse* response);
};

#endif



#ifndef SERVICE_IMPLEMENTATIONS_NAVI_CUT_IN_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_NAVI_CUT_IN_SERVICE_H

#include <service/NaviCutInService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */

class NaviCutInService final : public apb::api::navicutin::NaviCutInService::Service, public CommonService {
public:
    explicit NaviCutInService(Database &db);
    ~NaviCutInService();

    NaviCutInService(const NaviCutInService &other) = delete;
    NaviCutInService &operator =(const NaviCutInService &other) = delete;

    ::grpc::Status RegisterPlayed(::grpc::ServerContext* context,
                                  const ::apb::api::navicutin::RegisterPlayedRequest* request,
                                  ::apb::api::navicutin::RegisterPlayedResponse* response) override;

private:
    void RegisterPlayedImpl(int64_t userId,
                            const ::apb::api::navicutin::RegisterPlayedRequest* request,
                            ::apb::api::navicutin::RegisterPlayedResponse* response);
};

#endif




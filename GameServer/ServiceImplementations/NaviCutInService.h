#ifndef SERVICE_IMPLEMENTATIONS_NAVI_CUT_IN_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_NAVI_CUT_IN_SERVICE_H

#include <service/NaviCutInService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */

class NaviCutInService final : public apb::api::navicutin::NaviCutInService, public CommonService {
public:
    explicit NaviCutInService(Database &db);
    ~NaviCutInService();

    NaviCutInService(const NaviCutInService &other) = delete;
    NaviCutInService &operator =(const NaviCutInService &other) = delete;

    void RegisterPlayed(::google::protobuf::RpcController* controller,
                        const ::apb::api::navicutin::RegisterPlayedRequest* request,
                        ::apb::api::navicutin::RegisterPlayedResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void RegisterPlayedImpl(UserContext &user,
                            const ::apb::api::navicutin::RegisterPlayedRequest* request,
                            ::apb::api::navicutin::RegisterPlayedResponse* response);
};

#endif




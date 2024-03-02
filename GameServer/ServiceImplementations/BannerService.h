#ifndef SERVICE_IMPLEMENTATIONS_BANNER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_BANNER_SERVICE_H

#include <service/BannerService.pb.h>

#include <ServiceImplementations/CommonService.h>

class BannerService final : public apb::api::banner::BannerService, public CommonService {
public:
    explicit BannerService(Database &db);
    ~BannerService();

    BannerService(const BannerService &other) = delete;
    BannerService &operator =(const BannerService &other) = delete;

    void GetMamaBanner(::google::protobuf::RpcController* controller,
                       const ::apb::api::banner::GetMamaBannerRequest* request,
                       ::apb::api::banner::GetMamaBannerResponse* response,
                       ::google::protobuf::Closure* done) override;

private:
    void GetMamaBannerImpl(UserContext &user,
                           const ::apb::api::banner::GetMamaBannerRequest* request,
                           ::apb::api::banner::GetMamaBannerResponse* response);
};

#endif




#ifndef SERVICE_IMPLEMENTATIONS_BANNER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_BANNER_SERVICE_H

#include <service/BannerService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class BannerService final : public apb::api::banner::BannerService::Service, public CommonService {
public:
    explicit BannerService(Database &db);
    ~BannerService();

    BannerService(const BannerService &other) = delete;
    BannerService &operator =(const BannerService &other) = delete;

    ::grpc::Status GetMamaBanner(::grpc::ServerContext* context,
                                 const ::apb::api::banner::GetMamaBannerRequest* request,
                                 ::apb::api::banner::GetMamaBannerResponse* response) override;
private:
    void GetMamaBannerImpl(int64_t userId,
                           const ::apb::api::banner::GetMamaBannerRequest* request,
                           ::apb::api::banner::GetMamaBannerResponse* response);
};

#endif




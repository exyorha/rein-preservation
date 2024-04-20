#ifndef SERVICE_IMPLEMENTATIONS_SHOP_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_SHOP_SERVICE_H

#include "DataModel/UserContext.h"
#include <service/ShopService.pb.h>

#include <ServiceImplementations/CommonService.h>

class ShopService final : public apb::api::shop::ShopService, public CommonService {
public:
    explicit ShopService(Database &db);
    ~ShopService();

    ShopService(const ShopService &other) = delete;
    ShopService &operator =(const ShopService &other) = delete;

    void Buy(::google::protobuf::RpcController* controller,
                        const ::apb::api::shop::BuyRequest* request,
                        ::apb::api::shop::BuyResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RefreshUserData(::google::protobuf::RpcController* controller,
                            const ::apb::api::shop::RefreshRequest* request,
                            ::apb::api::shop::RefreshResponse* response,
                            ::google::protobuf::Closure* done) override;

private:
    void BuyImpl(UserContext &user, const ::apb::api::shop::BuyRequest* request, ::apb::api::shop::BuyResponse* response);

    void RefreshUserDataImpl(UserContext &user, const ::apb::api::shop::RefreshRequest* request, ::apb::api::shop::RefreshResponse* response);
};

#endif

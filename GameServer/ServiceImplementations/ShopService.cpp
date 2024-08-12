#include "ServiceImplementations/ShopService.h"

ShopService::ShopService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

ShopService::~ShopService() = default;

void ShopService::Buy(::google::protobuf::RpcController* controller,
                    const ::apb::api::shop::BuyRequest* request,
                    ::apb::api::shop::BuyResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("ShopService::Buy", controller, request, response, done, &ShopService::BuyImpl);
}

void ShopService::RefreshUserData(::google::protobuf::RpcController* controller,
                        const ::apb::api::shop::RefreshRequest* request,
                        ::apb::api::shop::RefreshResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("ShopService::RefreshUserData", controller, request, response, done, &ShopService::RefreshUserDataImpl);
}

void ShopService::BuyImpl(UserContext &user, const ::apb::api::shop::BuyRequest* request, ::apb::api::shop::BuyResponse* response) {
    /*
     * TODO: overflow management (it's lacking in general).
     */

    for(const auto &item: request->shop_items()) {
        for(int32_t index = 0; index < item.second; index++) {
            user.buyShopItem(request->shop_id(), item.first);
        }
    }
}

void ShopService::RefreshUserDataImpl(UserContext &user, const ::apb::api::shop::RefreshRequest* request, ::apb::api::shop::RefreshResponse* response) {

}

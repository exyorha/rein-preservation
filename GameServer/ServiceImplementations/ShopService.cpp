#include "ServiceImplementations/ShopService.h"

ShopService::ShopService(Database &db) : CommonService(db) {

}

ShopService::~ShopService() = default;

void ShopService::RefreshUserData(::google::protobuf::RpcController* controller,
                        const ::apb::api::shop::RefreshRequest* request,
                        ::apb::api::shop::RefreshResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("ShopService::RefreshUserData", controller, request, response, done, &ShopService::RefreshUserDataImpl);
}


void ShopService::RefreshUserDataImpl(UserContext &user, const ::apb::api::shop::RefreshRequest* request, ::apb::api::shop::RefreshResponse* response) {

}

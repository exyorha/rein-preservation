#include <ServiceImplementations/BannerService.h>

#include <DataModel/Sqlite/Statement.h>

BannerService::BannerService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

BannerService::~BannerService() = default;

void BannerService::GetMamaBanner(::google::protobuf::RpcController* controller,
                       const ::apb::api::banner::GetMamaBannerRequest* request,
                       ::apb::api::banner::GetMamaBannerResponse* response,
                       ::google::protobuf::Closure* done) {

    return inChangesetCall("BannerService::GetMamaBanner", controller, request, response, done, &BannerService::GetMamaBannerImpl);
}

void BannerService::GetMamaBannerImpl(
    UserContext &user, const ::apb::api::banner::GetMamaBannerRequest* request,
    ::apb::api::banner::GetMamaBannerResponse* response) {

}


#include <ServiceImplementations/BannerService.h>

#include <DataModel/Sqlite/Statement.h>

BannerService::BannerService(Database &db) : CommonService(db) {

}

BannerService::~BannerService() = default;

::grpc::Status BannerService::GetMamaBanner(
    ::grpc::ServerContext* context, const ::apb::api::banner::GetMamaBannerRequest* request,
    ::apb::api::banner::GetMamaBannerResponse* response) {

    return inChangesetCall("BannerService::GetMamaBanner", context, request, response, &BannerService::GetMamaBannerImpl);
}

void BannerService::GetMamaBannerImpl(
    UserContext &user, const ::apb::api::banner::GetMamaBannerRequest* request,
    ::apb::api::banner::GetMamaBannerResponse* response) {

}


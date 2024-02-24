#include <ServiceImplementations/DokanService.h>

#include <DataModel/Sqlite/Statement.h>

DokanService::DokanService(Database &db) : CommonService(db) {

}

DokanService::~DokanService() = default;

::grpc::Status DokanService::RegisterDokanConfirmed(
    ::grpc::ServerContext* context,
    const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
    ::apb::api::dokan::RegisterDokanConfirmedResponse* response) {
    return inChangesetCall("DokanService::RegisterDokanConfirmed", context, request, response, &DokanService::RegisterDokanConfirmedImpl);
}

void DokanService::RegisterDokanConfirmedImpl(
    UserContext &user,
    const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
    ::apb::api::dokan::RegisterDokanConfirmedResponse* response) {

    for(auto dokan: request->dokan_id()) {
        user.registerDokanConfirmed(dokan);
    }

}

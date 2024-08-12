#include <ServiceImplementations/DokanService.h>

#include <DataModel/Sqlite/Statement.h>

DokanService::DokanService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

DokanService::~DokanService() = default;

void DokanService::RegisterDokanConfirmed(::google::protobuf::RpcController* controller,
                            const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
                            ::apb::api::dokan::RegisterDokanConfirmedResponse* response,
                            ::google::protobuf::Closure* done) {
    return inChangesetCall("DokanService::RegisterDokanConfirmed", controller, request, response, done,
                           &DokanService::RegisterDokanConfirmedImpl);
}

void DokanService::RegisterDokanConfirmedImpl(
    UserContext &user,
    const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
    ::apb::api::dokan::RegisterDokanConfirmedResponse* response) {

    for(auto dokan: request->dokan_id()) {
        user.registerDokanConfirmed(dokan);
    }

}

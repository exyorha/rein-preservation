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
    int64_t userId,
    const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
    ::apb::api::dokan::RegisterDokanConfirmedResponse* response) {

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_dokan (
            user_id,
            dokan_id,
            display_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, dokan_id) DO NOTHING
    )SQL");
    query->bind(1, userId);

    for(auto dokan: request->dokan_id()) {
        query->bind(2, dokan);
        query->exec();

        query->reset();
    }
}

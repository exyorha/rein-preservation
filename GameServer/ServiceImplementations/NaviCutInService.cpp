#include <ServiceImplementations/NaviCutInService.h>

#include <DataModel/Sqlite/Statement.h>

NaviCutInService::NaviCutInService(Database &db) :CommonService(db) {

}

NaviCutInService::~NaviCutInService() = default;


::grpc::Status NaviCutInService::RegisterPlayed(::grpc::ServerContext* context, const ::apb::api::navicutin::RegisterPlayedRequest* request,
                                        ::apb::api::navicutin::RegisterPlayedResponse* response) {

    return inChangesetCall("NaviCutInService::RegisterPlayed", context, request, response, &NaviCutInService::RegisterPlayedImpl);
}

void NaviCutInService::RegisterPlayedImpl(int64_t userId, const ::apb::api::navicutin::RegisterPlayedRequest* request,
                                                    ::apb::api::navicutin::RegisterPlayedResponse* response) {

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_navi_cut_in (
            user_id,
            navi_cut_in_id,
            play_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, navi_cut_in_id) DO NOTHING
    )SQL");

    query->bind(1, userId);
    query->bind(2, request->navi_cut_id());
    query->exec();
}

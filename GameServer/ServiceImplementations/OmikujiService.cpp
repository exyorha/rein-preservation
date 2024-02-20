#include "ServiceImplementations/OmikujiService.h"

#include <DataModel/Sqlite/Statement.h>

OmikujiService::OmikujiService(Database &db) : CommonService(db) {

}

OmikujiService::~OmikujiService() = default;


::grpc::Status OmikujiService::OmikujiDraw(
    ::grpc::ServerContext* context,
    const ::apb::api::omikuji::OmikujiDrawRequest* request,
    ::apb::api::omikuji::OmikujiDrawResponse* response) {

    return inChangesetCall("OmikujiService::OmikujiDraw", context, request, response, &OmikujiService::OmikujiDrawImpl);
}

void OmikujiService::OmikujiDrawImpl(
    int64_t userId,
    const ::apb::api::omikuji::OmikujiDrawRequest* request,
    ::apb::api::omikuji::OmikujiDrawResponse* response) {

    auto draw = db().prepare(R"SQL(
        INSERT INTO i_user_omikuji (
            user_id,
            omikuji_id,
            latest_draw_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, omikuji_id) DO UPDATE SET latest_draw_datetime = excluded.latest_draw_datetime
    )SQL");
    draw->bind(1, userId);
    draw->bind(2, request->omikuji_id());
    draw->exec();
}

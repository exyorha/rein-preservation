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
    UserContext &user,
    const ::apb::api::omikuji::OmikujiDrawRequest* request,
    ::apb::api::omikuji::OmikujiDrawResponse* response) {

    user.registerOmikujiDraw(request->omikuji_id());

}

#include "ServiceImplementations/OmikujiService.h"

#include <DataModel/Sqlite/Statement.h>

OmikujiService::OmikujiService(Database &db) : CommonService(db) {

}

OmikujiService::~OmikujiService() = default;


void OmikujiService::OmikujiDraw(::google::protobuf::RpcController* controller,
                            const ::apb::api::omikuji::OmikujiDrawRequest* request,
                            ::apb::api::omikuji::OmikujiDrawResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("OmikujiService::OmikujiDraw", controller, request, response, done, &OmikujiService::OmikujiDrawImpl);
}

void OmikujiService::OmikujiDrawImpl(
    UserContext &user,
    const ::apb::api::omikuji::OmikujiDrawRequest* request,
    ::apb::api::omikuji::OmikujiDrawResponse* response) {

    user.registerOmikujiDraw(request->omikuji_id());

}

#include <ServiceImplementations/MaterialService.h>

MaterialService::MaterialService(Database &db) : CommonService(db) {

}

MaterialService::~MaterialService() = default;

void MaterialService::Sell(::google::protobuf::RpcController* controller,
                        const ::apb::api::material::SellRequest* request,
                        ::apb::api::material::SellResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("MaterialService::Sell", controller, request, response, done, &MaterialService::SellImpl);
}

void MaterialService::SellImpl(
    UserContext &user,
    const ::apb::api::material::SellRequest* request,
    ::apb::api::material::SellResponse* response) {


    for(const auto &possession: request->material_possession()) {
        user.sellMaterial(possession.material_id(), possession.count());
    }
}


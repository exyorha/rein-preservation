#include "service/GachaService.pb.h"
#include <ServiceImplementations/GachaService.h>

GachaService::GachaService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

GachaService::~GachaService() = default;


void GachaService::GetGachaList(::google::protobuf::RpcController* controller,
                        const ::apb::api::gacha::GetGachaListRequest* request,
                        ::apb::api::gacha::GetGachaListResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("GachaService::GetGachaList", controller, request, response, done, &GachaService::GetGachaListImpl);
}

void GachaService::GetGacha(::google::protobuf::RpcController* controller,
                        const ::apb::api::gacha::GetGachaRequest* request,
                        ::apb::api::gacha::GetGachaResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("GachaService::GetGacha", controller, request, response, done, &GachaService::GetGachaImpl);
}

void GachaService::GetRewardGacha(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gacha::GetRewardGachaResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("GachaService::GetRewardGacha", controller, request, response, done, &GachaService::GetRewardGachaImpl);
}


void GachaService::GetGachaListImpl(UserContext &user,
                                              const ::apb::api::gacha::GetGachaListRequest* request, ::apb::api::gacha::GetGachaListResponse* response) {

    // TODO (if possible)

    throw std::runtime_error("not implemented! failing the call to prevent the client locking up due to the empty list");
}

void GachaService::GetGachaImpl(UserContext &user, const apb::api::gacha::GetGachaRequest* request,
                                      ::apb::api::gacha::GetGachaResponse* response) {
    // TODO (if possible)
}

void GachaService::GetRewardGachaImpl(UserContext &user, const ::google::protobuf::Empty* request,
                                      ::apb::api::gacha::GetRewardGachaResponse* response) {
    // TODO (if possible)
}

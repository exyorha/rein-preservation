#include "service/GachaService.pb.h"
#include <ServiceImplementations/GachaService.h>

GachaService::GachaService(Database &db) :CommonService(db) {

}

GachaService::~GachaService() = default;


::grpc::Status GachaService::GetGachaList(
    ::grpc::ServerContext* context, const ::apb::api::gacha::GetGachaListRequest* request, ::apb::api::gacha::GetGachaListResponse* response) {

    return inChangesetCall("GachaService::GetGachaList", context, request, response, &GachaService::GetGachaListImpl);
}

::grpc::Status GachaService::GetGacha(
    ::grpc::ServerContext* context, const ::apb::api::gacha::GetGachaRequest* request, ::apb::api::gacha::GetGachaResponse* response) {

    return inChangesetCall("GachaService::GetGacha", context, request, response, &GachaService::GetGachaImpl);
}

::grpc::Status GachaService::GetRewardGacha(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                        ::apb::api::gacha::GetRewardGachaResponse* response) {

    return inChangesetCall("GachaService::GetRewardGacha", context, request, response, &GachaService::GetRewardGachaImpl);
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

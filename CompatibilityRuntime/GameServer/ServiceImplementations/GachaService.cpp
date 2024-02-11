#include <ServiceImplementations/GachaService.h>

GachaService::GachaService(Database &db) :CommonService(db) {

}

GachaService::~GachaService() = default;


::grpc::Status GachaService::GetRewardGacha(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                        ::apb::api::gacha::GetRewardGachaResponse* response) {

    return inChangesetCall("GachaService::GetRewardGacha", context, request, response, &GachaService::GetRewardGachaImpl);
}

void GachaService::GetRewardGachaImpl(int64_t userId, const ::google::protobuf::Empty* request,
                                                    ::apb::api::gacha::GetRewardGachaResponse* response) {

}

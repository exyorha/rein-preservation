#ifndef SERVICE_IMPLEMENTATIONS_GACHA_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GACHA_SERVICE_H

#include <service/GachaService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class GachaService final : public apb::api::gacha::GachaService::Service, public CommonService {
public:
    explicit GachaService(Database &db);
    ~GachaService();

    GachaService(const GachaService &other) = delete;
    GachaService &operator =(const GachaService &other) = delete;

    ::grpc::Status GetRewardGacha(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::apb::api::gacha::GetRewardGachaResponse* response) override;

private:
    void GetRewardGachaImpl(int64_t userId, const ::google::protobuf::Empty* request, ::apb::api::gacha::GetRewardGachaResponse* response);


};

#endif



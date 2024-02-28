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

    ::grpc::Status GetGachaList(
        ::grpc::ServerContext* context, const ::apb::api::gacha::GetGachaListRequest* request, ::apb::api::gacha::GetGachaListResponse* response) override;

    ::grpc::Status GetGacha(::grpc::ServerContext* context,
                            const ::apb::api::gacha::GetGachaRequest* request,
                            ::apb::api::gacha::GetGachaResponse* response) override;

    ::grpc::Status GetRewardGacha(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::apb::api::gacha::GetRewardGachaResponse* response) override;

private:
    void GetGachaListImpl(
        UserContext &user, const ::apb::api::gacha::GetGachaListRequest* request, ::apb::api::gacha::GetGachaListResponse* response);

    void GetGachaImpl(
        UserContext &user, const ::apb::api::gacha::GetGachaRequest* request, ::apb::api::gacha::GetGachaResponse* response);

    void GetRewardGachaImpl(UserContext &user, const ::google::protobuf::Empty* request, ::apb::api::gacha::GetRewardGachaResponse* response);


};

#endif



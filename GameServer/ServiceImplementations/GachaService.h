#ifndef SERVICE_IMPLEMENTATIONS_GACHA_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GACHA_SERVICE_H

#include <service/GachaService.pb.h>

#include <ServiceImplementations/CommonService.h>

class GachaService final : public apb::api::gacha::GachaService, public CommonService {
public:
    explicit GachaService(Database &db);
    ~GachaService();

    GachaService(const GachaService &other) = delete;
    GachaService &operator =(const GachaService &other) = delete;

    void GetGachaList(::google::protobuf::RpcController* controller,
                        const ::apb::api::gacha::GetGachaListRequest* request,
                        ::apb::api::gacha::GetGachaListResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetGacha(::google::protobuf::RpcController* controller,
                        const ::apb::api::gacha::GetGachaRequest* request,
                        ::apb::api::gacha::GetGachaResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetRewardGacha(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gacha::GetRewardGachaResponse* response,
                            ::google::protobuf::Closure* done) override;
private:
    void GetGachaListImpl(
        UserContext &user, const ::apb::api::gacha::GetGachaListRequest* request, ::apb::api::gacha::GetGachaListResponse* response);

    void GetGachaImpl(
        UserContext &user, const ::apb::api::gacha::GetGachaRequest* request, ::apb::api::gacha::GetGachaResponse* response);

    void GetRewardGachaImpl(UserContext &user, const ::google::protobuf::Empty* request, ::apb::api::gacha::GetRewardGachaResponse* response);


};

#endif



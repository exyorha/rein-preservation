#ifndef SERVICE_IMPLEMENTATIONS_LOGIN_BONUS_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_LOGIN_BONUS_SERVICE_H

#include <service/LoginBonusService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class LoginBonusService final : public apb::api::loginBonus::LoginBonusService::Service, public CommonService {
public:
    explicit LoginBonusService(Database &db);
    ~LoginBonusService();

    LoginBonusService(const LoginBonusService &other) = delete;
    LoginBonusService &operator =(const LoginBonusService &other) = delete;

    ::grpc::Status ReceiveStamp(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::apb::api::loginBonus::ReceiveStampResponse* response) override;

private:
    void ReceiveStampImpl(UserContext &user,
        const ::google::protobuf::Empty* request,
        ::apb::api::loginBonus::ReceiveStampResponse* response);

};

#endif



#ifndef SERVICE_IMPLEMENTATIONS_USER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_USER_SERVICE_H

#include <service/UserService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class UserService final : public apb::api::user::UserService::Service, public CommonService {
public:
    explicit UserService(Database &db);
    ~UserService();

    UserService(const UserService &other) = delete;
    UserService &operator =(const UserService &other) = delete;

    ::grpc::Status RegisterUser(::grpc::ServerContext* context,
                                const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) override;

    ::grpc::Status Auth(::grpc::ServerContext* context, const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) override;

    ::grpc::Status GameStart(::grpc::ServerContext* context, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response) override;

    ::grpc::Status SetUserName(::grpc::ServerContext* context, const ::apb::api::user::SetUserNameRequest* request,
                               ::apb::api::user::SetUserNameResponse* response) override;

    ::grpc::Status SetUserMessage(::grpc::ServerContext* context,
                                  const ::apb::api::user::SetUserMessageRequest* request, ::apb::api::user::SetUserMessageResponse* response) override;

    ::grpc::Status SetUserFavoriteCostumeId(::grpc::ServerContext* context, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                            ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) override;

    ::grpc::Status GetAndroidArgs(::grpc::ServerContext* context,
                                  const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) override;

private:
    void RegisterUserImpl(const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response);

    void AuthImpl(const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response);

    void GameStartImpl(int64_t userId, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response);

    void SetUserNameImpl(int64_t userId, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response);

    void SetUserMessageImpl(int64_t userId, const ::apb::api::user::SetUserMessageRequest* request, ::apb::api::user::SetUserMessageResponse* response);

    void SetUserFavoriteCostumeIdImpl(int64_t userId, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                      ::apb::api::user::SetUserFavoriteCostumeIdResponse* response);

    void GetAndroidArgsImpl(const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response);

};

#endif

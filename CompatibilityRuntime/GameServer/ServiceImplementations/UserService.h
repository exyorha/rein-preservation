#ifndef SERVICE_IMPLEMENTATIONS_USER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_USER_SERVICE_H

#include <service/UserService.grpc.pb.h>

class UserService final : public apb::api::user::UserService::Service {
public:
    UserService();
    ~UserService();

    UserService(const UserService &other) = delete;
    UserService &operator =(const UserService &other) = delete;

    ::grpc::Status RegisterUser(::grpc::ServerContext* context, const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) override;
    ::grpc::Status Auth(::grpc::ServerContext* context, const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) override;
    ::grpc::Status GetAndroidArgs(::grpc::ServerContext* context, const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) override;

    ::grpc::Status SetUserName(::grpc::ServerContext* context, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response) override;

    ::grpc::Status GameStart(::grpc::ServerContext* context, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response) override;

};

#endif

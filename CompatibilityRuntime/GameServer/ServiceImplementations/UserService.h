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

};

#endif

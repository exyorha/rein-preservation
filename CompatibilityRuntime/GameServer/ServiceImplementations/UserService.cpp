#include "UserService.h"
#include <grpcpp/support/status.h>

UserService::UserService() = default;

UserService::~UserService() = default;

::grpc::Status UserService::RegisterUser(::grpc::ServerContext* context, const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) {
    printf("UserService::RegisterUser: %s\n", request->DebugString().c_str());

    return grpc::Status(grpc::UNIMPLEMENTED, "Not implemented");
}

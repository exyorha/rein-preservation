#include "UserService.h"
#include <grpcpp/support/status.h>

UserService::UserService() = default;

UserService::~UserService() = default;

::grpc::Status UserService::RegisterUser(::grpc::ServerContext* context,
                                         const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) {
    printf("UserService::RegisterUser: %s\n", request->DebugString().c_str());

    response->set_user_id(1);
    response->set_signature("signature");

    return grpc::Status::OK;
}

::grpc::Status UserService::GetAndroidArgs(::grpc::ServerContext* context,
                                           const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {
    printf("UserService::GetAndroidArgs: %s\n", request->DebugString().c_str());

    return grpc::Status::OK;

}

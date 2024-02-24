#include "UserService.h"

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

#include <grpcpp/support/status.h>

UserService::UserService(Database &db) : CommonService(db) {

}

UserService::~UserService() = default;

::grpc::Status UserService::RegisterUser(::grpc::ServerContext* context,
                                         const ::apb::api::user::RegisterUserRequest* request,
                                         ::apb::api::user::RegisterUserResponse* response) {

    return inCall("UserService::RegisterUser", context, request, response, &UserService::RegisterUserImpl);

}

void UserService::RegisterUserImpl(const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) {

    response->set_user_id(global().registerUser());

    response->set_signature("signature");
}

::grpc::Status UserService::Auth(::grpc::ServerContext* context, const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) {
    return inCall("UserService::Auth", context, request, response, &UserService::AuthImpl);
}

void UserService::AuthImpl(const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) {

    int64_t userId;
    std::string sessionKey;
    time_t expiration;

    global().authenticate(userId, sessionKey, expiration);

    response->set_user_id(userId);
    response->set_sessionkey(sessionKey);

    google::protobuf::Timestamp &expire = *response->mutable_expire_datetime();
    expire.set_seconds(expiration);
}

::grpc::Status UserService::GameStart(::grpc::ServerContext* context, const ::google::protobuf::Empty * request,
                                      ::apb::api::user::GameStartResponse* response) {

    return inChangesetCall("UserService::GameStart", context, request, response, &UserService::GameStartImpl);
}

void UserService::GameStartImpl(UserContext &user, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response) {

    user.gameStart();

}

::grpc::Status UserService::SetUserName(
    ::grpc::ServerContext* context, const ::apb::api::user::SetUserNameRequest* request,
    ::apb::api::user::SetUserNameResponse* response) {

    return inChangesetCall("UserService::SetUserName", context, request, response, &UserService::SetUserNameImpl);
}

void UserService::SetUserNameImpl(UserContext &user, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response) {

    user.setUserName(request->name());
}


::grpc::Status UserService::SetUserMessage(::grpc::ServerContext* context, const ::apb::api::user::SetUserMessageRequest* request,
                                           ::apb::api::user::SetUserMessageResponse* response) {

    return inChangesetCall("UserService::SetUserMessage", context, request, response, &UserService::SetUserMessageImpl);
}


void UserService::SetUserMessageImpl(UserContext &user, const ::apb::api::user::SetUserMessageRequest* request,
                                     ::apb::api::user::SetUserMessageResponse* response) {

    user.setUserMessage(request->message());
}

::grpc::Status UserService::SetUserFavoriteCostumeId(::grpc::ServerContext* context,
                                                     const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                                     ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) {

    return inChangesetCall("UserService::SetUserFavoriteCostumeId", context, request, response, &UserService::SetUserFavoriteCostumeIdImpl);
}

void UserService::SetUserFavoriteCostumeIdImpl(UserContext &user, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                               ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) {

    user.setUserFavoriteCostume(request->favorite_costume_id());
}

::grpc::Status UserService::GetAndroidArgs(::grpc::ServerContext* context,
                                           const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {

    return inCall("UserService::GetAndroidArgs", context, request, response, &UserService::GetAndroidArgsImpl);
}

void UserService::GetAndroidArgsImpl(const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {
    (void)request;
    (void)response;

}

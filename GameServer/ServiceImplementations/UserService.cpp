#include "UserService.h"

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

UserService::UserService(Database &db) : CommonService(db) {

}

UserService::~UserService() = default;

void UserService::RegisterUser(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::RegisterUserRequest* request,
                        ::apb::api::user::RegisterUserResponse* response,
                        ::google::protobuf::Closure* done) {

    return inCall("UserService::RegisterUser", controller, request, response, done, &UserService::RegisterUserImpl);

}

void UserService::RegisterUserImpl(const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response) {

    response->set_user_id(global().registerUser());

    response->set_signature("signature");
}

void UserService::Auth(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::AuthUserRequest* request,
                        ::apb::api::user::AuthUserResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inCall("UserService::Auth", controller, request, response, done, &UserService::AuthImpl);
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

void UserService::GameStart(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::user::GameStartResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::GameStart", controller, request, response, done,
                           &UserService::GameStartImpl);
}

void UserService::GameStartImpl(UserContext &user, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response) {

    user.gameStart();

}

void UserService::SetUserName(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserNameRequest* request,
                        ::apb::api::user::SetUserNameResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::SetUserName", controller, request, response, done, &UserService::SetUserNameImpl);
}

void UserService::SetUserNameImpl(UserContext &user, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response) {

    user.setUserName(request->name());
}


void UserService::SetUserMessage(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserMessageRequest* request,
                        ::apb::api::user::SetUserMessageResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inChangesetCall("UserService::SetUserMessage", controller, request, response, done, &UserService::SetUserMessageImpl);
}


void UserService::SetUserMessageImpl(UserContext &user, const ::apb::api::user::SetUserMessageRequest* request,
                                     ::apb::api::user::SetUserMessageResponse* response) {

    user.setUserMessage(request->message());
}

void UserService::SetUserFavoriteCostumeId(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                        ::apb::api::user::SetUserFavoriteCostumeIdResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::SetUserFavoriteCostumeId", controller, request, response, done,
                           &UserService::SetUserFavoriteCostumeIdImpl);
}

void UserService::SetUserFavoriteCostumeIdImpl(UserContext &user, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                               ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) {

    user.setUserFavoriteCostume(request->favorite_costume_id());
}


void UserService::GetUserProfile(::google::protobuf::RpcController* controller,
                       const ::apb::api::user::GetUserProfileRequest* request,
                       ::apb::api::user::GetUserProfileResponse* response,
                       ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::GetUserProfile", controller, request, response, done, &UserService::GetUserProfileImpl);
}

void UserService::GetUserProfileImpl(UserContext &user,
                        const ::apb::api::user::GetUserProfileRequest* request, ::apb::api::user::GetUserProfileResponse* response) {

    user.getUserProfile(request->player_id(), response);
}

void UserService::GetAndroidArgs(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::GetAndroidArgsRequest* request,
                        ::apb::api::user::GetAndroidArgsResponse* response,
                        ::google::protobuf::Closure* done) {

    return inCall("UserService::GetAndroidArgs", controller, request, response, done, &UserService::GetAndroidArgsImpl);
}

void UserService::GetAndroidArgsImpl(const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {
    (void)request;
    (void)response;

}

void UserService::GetBackupToken(::google::protobuf::RpcController* controller,
                    const ::apb::api::user::GetBackupTokenRequest* request,
                    ::apb::api::user::GetBackupTokenResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::GetBackupToken", controller, request, response, done, &UserService::GetBackupTokenImpl);
}

void UserService::GetBackupTokenImpl(UserContext &user,
                                     const apb::api::user::GetBackupTokenRequest *request,
                                     apb::api::user::GetBackupTokenResponse *response) {

    response->set_backup_token("not-a-token");
}

void UserService::CheckTransferSetting(::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Empty* request,
                    ::apb::api::user::CheckTransferSettingResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("UserService::CheckTransferSetting", controller, request, response, done, &UserService::CheckTransferSettingImpl);
}

void UserService::CheckTransferSettingImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request,
    ::apb::api::user::CheckTransferSettingResponse* response) {


}

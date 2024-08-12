#ifndef SERVICE_IMPLEMENTATIONS_USER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_USER_SERVICE_H

#include <service/UserService.pb.h>

#include <ServiceImplementations/CommonService.h>

class UserService final : public apb::api::user::UserService, public CommonService {
public:
    UserService(Database &db, const GameServerConfiguration &config);
    ~UserService();

    UserService(const UserService &other) = delete;
    UserService &operator =(const UserService &other) = delete;

    void RegisterUser(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::RegisterUserRequest* request,
                        ::apb::api::user::RegisterUserResponse* response,
                        ::google::protobuf::Closure* done) override;

    void Auth(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::AuthUserRequest* request,
                        ::apb::api::user::AuthUserResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GameStart(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::user::GameStartResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetUserName(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserNameRequest* request,
                        ::apb::api::user::SetUserNameResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetUserMessage(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserMessageRequest* request,
                        ::apb::api::user::SetUserMessageResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetUserFavoriteCostumeId(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                        ::apb::api::user::SetUserFavoriteCostumeIdResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetUserProfile(::google::protobuf::RpcController* controller,
                       const ::apb::api::user::GetUserProfileRequest* request,
                       ::apb::api::user::GetUserProfileResponse* response,
                       ::google::protobuf::Closure* done) override;

    void GetAndroidArgs(::google::protobuf::RpcController* controller,
                        const ::apb::api::user::GetAndroidArgsRequest* request,
                        ::apb::api::user::GetAndroidArgsResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetBackupToken(::google::protobuf::RpcController* controller,
                       const ::apb::api::user::GetBackupTokenRequest* request,
                       ::apb::api::user::GetBackupTokenResponse* response,
                       ::google::protobuf::Closure* done) override;

    void CheckTransferSetting(::google::protobuf::RpcController* controller,
                       const ::google::protobuf::Empty* request,
                       ::apb::api::user::CheckTransferSettingResponse* response,
                       ::google::protobuf::Closure* done) override;

private:
    void RegisterUserImpl(const ::apb::api::user::RegisterUserRequest* request, ::apb::api::user::RegisterUserResponse* response);

    void AuthImpl(const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response);

    void GameStartImpl(UserContext &user, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response);

    void SetUserNameImpl(UserContext &user, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response);

    void SetUserMessageImpl(UserContext &user, const ::apb::api::user::SetUserMessageRequest* request, ::apb::api::user::SetUserMessageResponse* response);

    void SetUserFavoriteCostumeIdImpl(UserContext &user, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                      ::apb::api::user::SetUserFavoriteCostumeIdResponse* response);

    void GetUserProfileImpl(UserContext &user,
                            const ::apb::api::user::GetUserProfileRequest* request, ::apb::api::user::GetUserProfileResponse* response);

    void GetAndroidArgsImpl(const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response);

    void GetBackupTokenImpl(UserContext &user,
                            const ::apb::api::user::GetBackupTokenRequest* request,
                       ::apb::api::user::GetBackupTokenResponse* response);

    void CheckTransferSettingImpl(
        UserContext &user,
        const ::google::protobuf::Empty* request,
        ::apb::api::user::CheckTransferSettingResponse* response);
};

#endif

#ifndef SERVICE_IMPLEMENTATIONS_FRIEND_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_FRIEND_SERVICE_H

#include <service/FriendService.pb.h>

#include <ServiceImplementations/CommonService.h>

class FriendService final : public apb::api::friends::FriendService, public CommonService {
public:
    explicit FriendService(Database &db);
    ~FriendService();

    FriendService(const FriendService &other) = delete;
    FriendService &operator =(const FriendService &other) = delete;

    void GetUser(::google::protobuf::RpcController* controller,
                        const ::apb::api::friends::GetUserRequest* request,
                        ::apb::api::friends::GetUserResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SearchRecommendedUsers(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty *request,
                        ::apb::api::friends::SearchRecommendedUsersResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetFriendList(::google::protobuf::RpcController* controller,
                        const ::apb::api::friends::GetFriendListRequest* request,
                        ::apb::api::friends::GetFriendListResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetFriendRequestList(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty *request,
                        ::apb::api::friends::GetFriendRequestListResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void GetUserImpl(
        UserContext &user, const ::apb::api::friends::GetUserRequest* request, ::apb::api::friends::GetUserResponse* response);

    void SearchRecommendedUsersImpl(
        UserContext &user, const ::google::protobuf::Empty *request, ::apb::api::friends::SearchRecommendedUsersResponse* response);

    void GetFriendListImpl(
        UserContext &user, const ::apb::api::friends::GetFriendListRequest* request, ::apb::api::friends::GetFriendListResponse* response);

    void GetFriendRequestListImpl(
        UserContext &user, const ::google::protobuf::Empty *request, ::apb::api::friends::GetFriendRequestListResponse* response);


};

#endif




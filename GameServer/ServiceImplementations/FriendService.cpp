#include <ServiceImplementations/FriendService.h>

FriendService::FriendService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

FriendService::~FriendService() = default;

void FriendService::GetUser(::google::protobuf::RpcController* controller,
                        const ::apb::api::friends::GetUserRequest* request,
                        ::apb::api::friends::GetUserResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("FriendService::GetUser", controller, request, response, done, &FriendService::GetUserImpl);
}

void FriendService::GetFriendList(::google::protobuf::RpcController* controller,
                        const ::apb::api::friends::GetFriendListRequest* request,
                        ::apb::api::friends::GetFriendListResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("FriendService::GetFriendList", controller, request, response, done, &FriendService::GetFriendListImpl);
}

void FriendService::GetFriendRequestList(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::friends::GetFriendRequestListResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("FriendService::GetFriendRequestList", controller, request, response, done, &FriendService::GetFriendRequestListImpl);
}

void FriendService::SearchRecommendedUsers(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::friends::SearchRecommendedUsersResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("FriendService::SearchRecommendedUsers", controller, request, response, done, &FriendService::SearchRecommendedUsersImpl);
}

void FriendService::GetUserImpl(
    UserContext &user, const ::apb::api::friends::GetUserRequest* request, ::apb::api::friends::GetUserResponse* response) {

}

void FriendService::GetFriendListImpl(
    UserContext &user, const ::apb::api::friends::GetFriendListRequest* request, ::apb::api::friends::GetFriendListResponse* response) {

}

void FriendService::GetFriendRequestListImpl(
    UserContext &user, const ::google::protobuf::Empty* request, ::apb::api::friends::GetFriendRequestListResponse* response) {

}

void FriendService::SearchRecommendedUsersImpl(
    UserContext &user, const ::google::protobuf::Empty* request, ::apb::api::friends::SearchRecommendedUsersResponse* response) {

}

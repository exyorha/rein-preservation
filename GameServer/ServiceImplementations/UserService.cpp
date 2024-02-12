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

    /*
     * Currently, we only maintain one user with the ID = 1, making it
     * impossible to lose user data. This is limited to this method and
     * the Auth method, though, the rest of the server properly handles
     * user IDs.
     */

    auto statement = db().prepare(R"SQL(
        INSERT INTO i_user (
            user_id, register_datetime
        )
        VALUES (
            1, current_net_timestamp()
        )
        ON CONFLICT (user_id) DO UPDATE SET user_id = user_id
        RETURNING user_id
    )SQL");

    while(statement->step()) {
        response->set_user_id(statement->columnInt(0));
    }

    response->set_signature("signature");
}

::grpc::Status UserService::Auth(::grpc::ServerContext* context, const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) {
    return inCall("UserService::Auth", context, request, response, &UserService::AuthImpl);
}

void UserService::AuthImpl(const ::apb::api::user::AuthUserRequest* request, ::apb::api::user::AuthUserResponse* response) {

    db().prepare("DELETE FROM internal_sessions WHERE user_id = 1")->exec();

    db().prepare("INSERT OR IGNORE INTO i_user (user_id, register_datetime) VALUES (1, current_net_timestamp())")->exec();

    auto makeSession = db().prepare(R"SQL(
        INSERT INTO internal_sessions (session_id, user_id, expire_datetime) VALUES (hex(randomblob(16)), ?, unixepoch('now', '10 hours'))
        RETURNING session_id, user_id, expire_datetime
    )SQL");

    makeSession->bind(1, 1); // The user ID is always '1' right now

    while(makeSession->step()) {
        response->set_sessionkey(makeSession->columnText(0));
        response->set_user_id(makeSession->columnInt(1));
        google::protobuf::Timestamp &expire = *response->mutable_expire_datetime();
        expire.set_seconds(makeSession->columnInt(2));
    }

    for(auto userInitQuery: {
        "INSERT OR IGNORE INTO i_user_status (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_gem (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_login (user_id) VALUES (?)", // note that login time is not updated here yet - that's GameStart!
        "INSERT OR IGNORE INTO i_user_profile (user_id) VALUES (?)"
    }) {
        auto query = db().prepare(userInitQuery);
        query->bind(1, response->user_id());
        query->exec();
    }
}

::grpc::Status UserService::GameStart(::grpc::ServerContext* context, const ::google::protobuf::Empty * request,
                                      ::apb::api::user::GameStartResponse* response) {

    return inChangesetCall("UserService::GameStart", context, request, response, &UserService::GameStartImpl);
}

void UserService::GameStartImpl(int64_t userId, const ::google::protobuf::Empty * request, ::apb::api::user::GameStartResponse* response) {

    for(auto userInitQuery: {
        /*
        * Set game_start_datetime if never initialized.
        */
        "UPDATE i_user SET game_start_datetime = current_net_timestamp() WHERE user_id = ? AND game_start_datetime < register_datetime",
        "INSERT OR IGNORE INTO i_user_tutorial_progress (user_id, tutorial_type, progress_phase, choice_id) VALUES (?, 1, 1, 0)",
        "INSERT OR IGNORE INTO i_user_setting (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_main_quest_main_flow_status (user_id) VALUES (?)"
    }) {
        auto query = db().prepare(userInitQuery);
        query->bind(1, userId);
        query->exec();
    }

    /*
     * TODO: IUserShopReplaceableLineup is updated here
     * TODO: IUserShopReplaceable is updated here
     */
}

::grpc::Status UserService::SetUserName(
    ::grpc::ServerContext* context, const ::apb::api::user::SetUserNameRequest* request,
    ::apb::api::user::SetUserNameResponse* response) {

    return inChangesetCall("UserService::SetUserName", context, request, response, &UserService::SetUserNameImpl);
}

void UserService::SetUserNameImpl(int64_t userId, const ::apb::api::user::SetUserNameRequest* request, ::apb::api::user::SetUserNameResponse* response) {

    auto statement = db().prepare("UPDATE i_user_profile SET name = ? WHERE user_id = ?");
    statement->bind(1, request->name());
    statement->bind(2, userId);

    statement->exec();
}


::grpc::Status UserService::SetUserMessage(::grpc::ServerContext* context, const ::apb::api::user::SetUserMessageRequest* request,
                                           ::apb::api::user::SetUserMessageResponse* response) {

    return inChangesetCall("UserService::SetUserMessage", context, request, response, &UserService::SetUserMessageImpl);
}


void UserService::SetUserMessageImpl(int64_t userId, const ::apb::api::user::SetUserMessageRequest* request,
                                     ::apb::api::user::SetUserMessageResponse* response) {

    auto statement = db().prepare("UPDATE i_user_profile SET message = ? WHERE user_id = ?");
    statement->bind(1, request->message());
    statement->bind(2, userId);

    statement->exec();
}

::grpc::Status UserService::SetUserFavoriteCostumeId(::grpc::ServerContext* context,
                                                     const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                                     ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) {

    return inChangesetCall("UserService::SetUserFavoriteCostumeId", context, request, response, &UserService::SetUserFavoriteCostumeIdImpl);
}

void UserService::SetUserFavoriteCostumeIdImpl(int64_t userId, const ::apb::api::user::SetUserFavoriteCostumeIdRequest* request,
                                               ::apb::api::user::SetUserFavoriteCostumeIdResponse* response) {

    auto statement = db().prepare("UPDATE i_user_profile SET favorite_costume_id = ? WHERE user_id = ?");
    statement->bind(1, request->favorite_costume_id());
    statement->bind(2, userId);

    statement->exec();
}

::grpc::Status UserService::GetAndroidArgs(::grpc::ServerContext* context,
                                           const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {

    return inCall("UserService::GetAndroidArgs", context, request, response, &UserService::GetAndroidArgsImpl);
}

void UserService::GetAndroidArgsImpl(const ::apb::api::user::GetAndroidArgsRequest* request, ::apb::api::user::GetAndroidArgsResponse* response) {
    (void)request;
    (void)response;

}

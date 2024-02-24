#include <ServiceImplementations/LoginBonusService.h>

LoginBonusService::LoginBonusService(Database &db) :CommonService(db) {

}

LoginBonusService::~LoginBonusService() = default;


::grpc::Status LoginBonusService::ReceiveStamp(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::apb::api::loginBonus::ReceiveStampResponse* response) {

    return inChangesetCall("LoginBonusService::ReceiveStamp", context, request, response, &LoginBonusService::ReceiveStampImpl);
}

void LoginBonusService::ReceiveStampImpl(UserContext &user, const ::google::protobuf::Empty* request,
                                         ::apb::api::loginBonus::ReceiveStampResponse* response) {

    /*
     * TODO.
     */
}


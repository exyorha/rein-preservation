#include <ServiceImplementations/LoginBonusService.h>

LoginBonusService::LoginBonusService(Database &db) :CommonService(db) {

}

LoginBonusService::~LoginBonusService() = default;


void LoginBonusService::ReceiveStamp(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::loginBonus::ReceiveStampResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("LoginBonusService::ReceiveStamp", controller, request, response, done,
                           &LoginBonusService::ReceiveStampImpl);
}

void LoginBonusService::ReceiveStampImpl(UserContext &user, const ::google::protobuf::Empty* request,
                                         ::apb::api::loginBonus::ReceiveStampResponse* response) {

    /*
     * TODO.
     */
}


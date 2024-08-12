#include <ServiceImplementations/NotificationService.h>

NotificationService::NotificationService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

NotificationService::~NotificationService() = default;


void NotificationService::GetHeaderNotification(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::notification::GetHeaderNotificationResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("NotificationService::GetHeaderNotification", controller, request, response,
                           done, &NotificationService::GetHeaderNotificationImpl);
}

void NotificationService::GetHeaderNotificationImpl(UserContext &user,
                                                    const ::google::protobuf::Empty* request,
                                                    ::apb::api::notification::GetHeaderNotificationResponse* response) {

    response->set_gift_not_receive_count(user.getNumberOfUnreceivedGifts());
}

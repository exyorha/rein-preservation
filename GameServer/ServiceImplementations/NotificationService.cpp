#include <ServiceImplementations/NotificationService.h>

NotificationService::NotificationService(Database &db) :CommonService(db) {

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

}

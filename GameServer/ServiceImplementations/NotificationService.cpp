#include <ServiceImplementations/NotificationService.h>

NotificationService::NotificationService(Database &db) :CommonService(db) {

}

NotificationService::~NotificationService() = default;


::grpc::Status NotificationService::GetHeaderNotification(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                        ::apb::api::notification::GetHeaderNotificationResponse* response) {

    return inChangesetCall("NotificationService::GetHeaderNotification", context, request, response, &NotificationService::GetHeaderNotificationImpl);
}

void NotificationService::GetHeaderNotificationImpl(UserContext &user,
                                                    const ::google::protobuf::Empty* request,
                                                    ::apb::api::notification::GetHeaderNotificationResponse* response) {

}

#ifndef SERVICE_IMPLEMENTATIONS_NOTIFICATION_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_NOTIFICATION_SERVICE_H

#include <service/NotificationService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class NotificationService final : public apb::api::notification::NotificationService::Service, public CommonService {
public:
    explicit NotificationService(Database &db);
    ~NotificationService();

    NotificationService(const NotificationService &other) = delete;
    NotificationService &operator =(const NotificationService &other) = delete;

    ::grpc::Status GetHeaderNotification(::grpc::ServerContext* context, const ::google::protobuf::Empty* request,
                                         ::apb::api::notification::GetHeaderNotificationResponse* response) override;

private:
    void GetHeaderNotificationImpl(UserContext &user,
                                   const ::google::protobuf::Empty* request, ::apb::api::notification::GetHeaderNotificationResponse* response);


};

#endif



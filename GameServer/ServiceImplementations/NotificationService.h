#ifndef SERVICE_IMPLEMENTATIONS_NOTIFICATION_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_NOTIFICATION_SERVICE_H

#include <service/NotificationService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class NotificationService final : public apb::api::notification::NotificationService, public CommonService {
public:
    NotificationService(Database &db, const GameServerConfiguration &config);
    ~NotificationService();

    NotificationService(const NotificationService &other) = delete;
    NotificationService &operator =(const NotificationService &other) = delete;

    void GetHeaderNotification(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::notification::GetHeaderNotificationResponse* response,
                            ::google::protobuf::Closure* done) override;

private:
    void GetHeaderNotificationImpl(UserContext &user,
                                   const ::google::protobuf::Empty* request, ::apb::api::notification::GetHeaderNotificationResponse* response);


};

#endif



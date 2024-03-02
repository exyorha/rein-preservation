#ifndef SERVICE_IMPLEMENTATIONS_DOKAN_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DOKAN_SERVICE_H

#include <service/DokanService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class DokanService final : public apb::api::dokan::DokanService, public CommonService {
public:
    explicit DokanService(Database &db);
    ~DokanService();

    DokanService(const DokanService &other) = delete;
    DokanService &operator =(const DokanService &other) = delete;

    void RegisterDokanConfirmed(::google::protobuf::RpcController* controller,
                            const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
                            ::apb::api::dokan::RegisterDokanConfirmedResponse* response,
                            ::google::protobuf::Closure* done) override;

private:

    void RegisterDokanConfirmedImpl(
        UserContext &user,
        const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
        ::apb::api::dokan::RegisterDokanConfirmedResponse* response);
};

#endif



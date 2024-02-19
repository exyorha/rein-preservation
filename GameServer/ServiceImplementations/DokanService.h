#ifndef SERVICE_IMPLEMENTATIONS_DOKAN_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DOKAN_SERVICE_H

#include <service/DokanService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class DokanService final : public apb::api::dokan::DokanService::Service, public CommonService {
public:
    explicit DokanService(Database &db);
    ~DokanService();

    DokanService(const DokanService &other) = delete;
    DokanService &operator =(const DokanService &other) = delete;

    ::grpc::Status RegisterDokanConfirmed(
        ::grpc::ServerContext* context,
        const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
        ::apb::api::dokan::RegisterDokanConfirmedResponse* response) override;

private:

    void RegisterDokanConfirmedImpl(
        int64_t userId,
        const ::apb::api::dokan::RegisterDokanConfirmedRequest* request,
        ::apb::api::dokan::RegisterDokanConfirmedResponse* response);
};

#endif



#ifndef SERVICE_IMPLEMENTATIONS_OMIKUJI_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_OMIKUJI_SERVICE_H

#include <service/OmikujiService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class OmikujiService final : public apb::api::omikuji::OmikujiService::Service, public CommonService {
public:
    explicit OmikujiService(Database &db);
    ~OmikujiService();

    OmikujiService(const OmikujiService &other) = delete;
    OmikujiService &operator =(const OmikujiService &other) = delete;

    ::grpc::Status OmikujiDraw(
        ::grpc::ServerContext* context,
        const ::apb::api::omikuji::OmikujiDrawRequest* request,
        ::apb::api::omikuji::OmikujiDrawResponse* response) override;

private:
    void OmikujiDrawImpl(
        UserContext &user,
        const ::apb::api::omikuji::OmikujiDrawRequest* request,
        ::apb::api::omikuji::OmikujiDrawResponse* response);

};

#endif

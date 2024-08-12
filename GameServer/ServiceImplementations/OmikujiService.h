#ifndef SERVICE_IMPLEMENTATIONS_OMIKUJI_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_OMIKUJI_SERVICE_H

#include <service/OmikujiService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class OmikujiService final : public apb::api::omikuji::OmikujiService, public CommonService {
public:
    OmikujiService(Database &db, const GameServerConfiguration &config);
    ~OmikujiService();

    OmikujiService(const OmikujiService &other) = delete;
    OmikujiService &operator =(const OmikujiService &other) = delete;

    void OmikujiDraw(::google::protobuf::RpcController* controller,
                            const ::apb::api::omikuji::OmikujiDrawRequest* request,
                            ::apb::api::omikuji::OmikujiDrawResponse* response,
                            ::google::protobuf::Closure* done) override;

private:
    void OmikujiDrawImpl(
        UserContext &user,
        const ::apb::api::omikuji::OmikujiDrawRequest* request,
        ::apb::api::omikuji::OmikujiDrawResponse* response);

};

#endif

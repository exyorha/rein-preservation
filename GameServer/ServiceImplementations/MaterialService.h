#ifndef SERVICE_IMPLEMENTATIONS_MATERIAL_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_MATERIAL_SERVICE_H

#include <service/MaterialService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class MaterialService final : public apb::api::material::MaterialService, public CommonService {
public:
    MaterialService(Database &db, const GameServerConfiguration &config);
    ~MaterialService();

    MaterialService(const MaterialService &other) = delete;
    MaterialService &operator =(const MaterialService &other) = delete;

    void Sell(::google::protobuf::RpcController* controller,
                        const ::apb::api::material::SellRequest* request,
                        ::apb::api::material::SellResponse* response,
                        ::google::protobuf::Closure* done) override;

private:

    void SellImpl(
        UserContext &user, const ::apb::api::material::SellRequest* request,
        ::apb::api::material::SellResponse* response);

};

#endif






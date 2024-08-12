#ifndef SERVICE_IMPLEMENTATIONS_CONSUMABLE_ITEM_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CONSUMABLE_ITEM_SERVICE_H

#include <service/ConsumableItemService.pb.h>

#include <ServiceImplementations/CommonService.h>

class ConsumableItemService final : public apb::api::consumableitem::ConsumableItemService, public CommonService {
public:
    ConsumableItemService(Database &db, const GameServerConfiguration &config);
    ~ConsumableItemService();

    ConsumableItemService(const ConsumableItemService &other) = delete;
    ConsumableItemService &operator =(const ConsumableItemService &other) = delete;

    void Sell(::google::protobuf::RpcController* controller,
              const ::apb::api::consumableitem::SellRequest* request,
              ::apb::api::consumableitem::SellResponse* response,
              ::google::protobuf::Closure* done) override;

private:

    void SellImpl(
        UserContext &user,
        const ::apb::api::consumableitem::SellRequest* request,
        ::apb::api::consumableitem::SellResponse* response);
};

#endif






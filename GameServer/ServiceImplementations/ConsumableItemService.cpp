#include <ServiceImplementations/ConsumableItemService.h>

ConsumableItemService::ConsumableItemService(Database &db) : CommonService(db) {

}

ConsumableItemService::~ConsumableItemService() = default;

void ConsumableItemService::Sell(::google::protobuf::RpcController* controller,
                        const ::apb::api::consumableitem::SellRequest* request,
                        ::apb::api::consumableitem::SellResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("ConsumableItemService::Sell", controller, request, response, done, &ConsumableItemService::SellImpl);
}

void ConsumableItemService::SellImpl(
    UserContext &user,
    const ::apb::api::consumableitem::SellRequest* request,
    ::apb::api::consumableitem::SellResponse* response) {


    for(const auto &possession: request->consumable_item_possession()) {
        user.sellConsumableItem(possession.consumable_item_id(), possession.count());
    }
}

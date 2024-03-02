#include <ServiceImplementations/DeckService.h>

#include <DataModel/Sqlite/Statement.h>

DeckService::DeckService(Database &db) : CommonService(db) {

}

DeckService::~DeckService() = default;

void DeckService::UpdateName(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::UpdateNameRequest* request,
                        ::apb::api::deck::UpdateNameResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::UpdateName", controller, request, response, done, &DeckService::UpdateNameImpl);
}

void DeckService::ReplaceDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceDeckRequest* request,
                        ::apb::api::deck::ReplaceDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::ReplaceDeck", controller, request, response, done, &DeckService::ReplaceDeckImpl);
}

void DeckService::RefreshDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshDeckPowerRequest* request,
                        ::apb::api::deck::RefreshDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::RefreshDeckPower", controller, request, response, done, &DeckService::RefreshDeckPowerImpl);
}


void DeckService::UpdateNameImpl(UserContext &user,
                                 const ::apb::api::deck::UpdateNameRequest* request,
                                 ::apb::api::deck::UpdateNameResponse* response) {

    user.updateDeckName(request->deck_type(), request->user_deck_number(), request->name());
}

void DeckService::ReplaceDeckImpl(UserContext &user,
                                  const ::apb::api::deck::ReplaceDeckRequest* request,
                                  ::apb::api::deck::ReplaceDeckResponse* response) {

    user.replaceDeck(request->deck_type(), request->user_deck_number(), request->has_deck() ? &request->deck() : nullptr);
}

void DeckService::RefreshDeckPowerImpl(UserContext &user,
                                       const ::apb::api::deck::RefreshDeckPowerRequest* request,
                                       ::apb::api::deck::RefreshDeckPowerResponse* response) {

    if(!request->has_deck_power())
        throw std::runtime_error("no deck power is specified in RefreshDeckPower");

    user.refreshDeckPower(request->deck_type(), request->user_deck_number(), request->deck_power());
}

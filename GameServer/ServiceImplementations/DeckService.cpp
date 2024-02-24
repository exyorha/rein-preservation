#include <ServiceImplementations/DeckService.h>

#include <DataModel/Sqlite/Statement.h>

DeckService::DeckService(Database &db) : CommonService(db) {

}

DeckService::~DeckService() = default;

::grpc::Status DeckService::UpdateName(::grpc::ServerContext* context,
                                        const ::apb::api::deck::UpdateNameRequest* request,
                                        ::apb::api::deck::UpdateNameResponse* response) {

    return inChangesetCall("DeckService::UpdateName", context, request, response, &DeckService::UpdateNameImpl);
}

::grpc::Status DeckService::ReplaceDeck(::grpc::ServerContext* context,
                                        const ::apb::api::deck::ReplaceDeckRequest* request,
                                        ::apb::api::deck::ReplaceDeckResponse* response) {

    return inChangesetCall("DeckService::ReplaceDeck", context, request, response, &DeckService::ReplaceDeckImpl);
}

::grpc::Status DeckService::RefreshDeckPower(::grpc::ServerContext* context,
                                        const ::apb::api::deck::RefreshDeckPowerRequest* request,
                                        ::apb::api::deck::RefreshDeckPowerResponse* response) {

    return inChangesetCall("DeckService::RefreshDeckPower", context, request, response, &DeckService::RefreshDeckPowerImpl);
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

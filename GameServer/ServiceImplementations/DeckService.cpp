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

void DeckService::CopyDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::CopyDeckRequest* request,
                        ::apb::api::deck::CopyDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::CopyDeck", controller, request, response, done, &DeckService::CopyDeckImpl);
}

void DeckService::RemoveDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RemoveDeckRequest* request,
                        ::apb::api::deck::RemoveDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::RemoveDeck", controller, request, response, done, &DeckService::RemoveDeckImpl);
}

void DeckService::RefreshDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshDeckPowerRequest* request,
                        ::apb::api::deck::RefreshDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::RefreshDeckPower", controller, request, response, done, &DeckService::RefreshDeckPowerImpl);
}

void DeckService::ReplaceMultiDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceMultiDeckRequest* request,
                        ::apb::api::deck::ReplaceMultiDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::ReplaceMultiDeck", controller, request, response, done, &DeckService::ReplaceMultiDeckImpl);
}

void DeckService::RefreshMultiDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshMultiDeckPowerRequest* request,
                        ::apb::api::deck::RefreshMultiDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::RefreshMultiDeckPower", controller, request, response, done, &DeckService::RefreshMultiDeckPowerImpl);
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

void DeckService::CopyDeckImpl(UserContext &user,
                               const ::apb::api::deck::CopyDeckRequest* request,
                               ::apb::api::deck::CopyDeckResponse* response) {

    apb::api::deck::Deck deck;
    user.readDeck(request->from_deck_type(), request->from_user_deck_number(), &deck);
    user.replaceDeck(request->to_deck_type(), request->to_user_deck_number(), &deck);
}

void DeckService::RemoveDeckImpl(UserContext &user,
                               const ::apb::api::deck::RemoveDeckRequest* request,
                               ::apb::api::deck::RemoveDeckResponse* response) {

    user.deleteDeck(request->deck_type(), request->user_deck_number());
}

void DeckService::RefreshDeckPowerImpl(UserContext &user,
                                       const ::apb::api::deck::RefreshDeckPowerRequest* request,
                                       ::apb::api::deck::RefreshDeckPowerResponse* response) {

    if(!request->has_deck_power())
        throw std::runtime_error("no deck power is specified in RefreshDeckPower");

    user.refreshDeckPower(request->deck_type(), request->user_deck_number(), request->deck_power());
}

void DeckService::ReplaceMultiDeckImpl(UserContext &user,
                                       const ::apb::api::deck::ReplaceMultiDeckRequest* request,
                                       ::apb::api::deck::ReplaceMultiDeckResponse* response) {

    for(const auto &deck: request->deck_detail()) {
        user.replaceDeck(
            deck.deck_type(),
            deck.user_deck_number(),
            deck.has_deck() ? &deck.deck() : nullptr
        );
    }
}

void DeckService::RefreshMultiDeckPowerImpl(UserContext &user,
                                       const ::apb::api::deck::RefreshMultiDeckPowerRequest* request,
                                       ::apb::api::deck::RefreshMultiDeckPowerResponse* response) {

    for(const auto &powerInfo: request->deck_power_info()) {
        user.refreshDeckPower(
            powerInfo.deck_type(),
            powerInfo.user_deck_number(),
            powerInfo.deck_power()
        );
    }
}



#include <ServiceImplementations/DeckService.h>

#include <DataModel/Sqlite/Statement.h>

DeckService::DeckService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

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

void DeckService::SetPvpDefenseDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::SetPvpDefenseDeckRequest* request,
                        ::apb::api::deck::SetPvpDefenseDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::SetPvpDefenseDeck", controller, request, response, done, &DeckService::SetPvpDefenseDeckImpl);
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

void DeckService::UpdateTripleDeckName(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::UpdateTripleDeckNameRequest* request,
                        ::apb::api::deck::UpdateTripleDeckNameResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::UpdateTripleDeckName", controller, request, response, done, &DeckService::UpdateTripleDeckNameImpl);
}

void DeckService::ReplaceTripleDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceTripleDeckRequest* request,
                        ::apb::api::deck::ReplaceTripleDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("DeckService::ReplaceTripleDeck", controller, request, response, done, &DeckService::ReplaceTripleDeckImpl);
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


void DeckService::SetPvpDefenseDeckImpl(UserContext &user,
                        const ::apb::api::deck::SetPvpDefenseDeckRequest* request,
                    ::apb::api::deck::SetPvpDefenseDeckResponse* response) {

    user.setPvpDefenseDeck(request->user_deck_number());
    /*
     * TODO: deck power
     */
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

void DeckService::UpdateTripleDeckNameImpl(UserContext &user,
                                       const ::apb::api::deck::UpdateTripleDeckNameRequest* request,
                                       ::apb::api::deck::UpdateTripleDeckNameResponse* response) {

    user.updateTripleDeckName(request->deck_type(), request->user_deck_number(), request->name());
}

void DeckService::ReplaceTripleDeckImpl(UserContext &user,
                                       const ::apb::api::deck::ReplaceTripleDeckRequest* request,
                                       ::apb::api::deck::ReplaceTripleDeckResponse* response) {

    int32_t deck1Number = 0;
    int32_t deck2Number = 0;
    int32_t deck3Number = 0;

    if(request->has_deck_detail_01()) {
        const auto &deck1 = request->deck_detail_01();
        user.replaceDeck(deck1.deck_type(), deck1.user_deck_number(), deck1.has_deck() ? &deck1.deck() : nullptr);
        deck1Number = deck1.user_deck_number();
    }

    if(request->has_deck_detail_02()) {
        const auto &deck2 = request->deck_detail_02();
        user.replaceDeck(deck2.deck_type(), deck2.user_deck_number(), deck2.has_deck() ? &deck2.deck() : nullptr);
        deck2Number = deck2.user_deck_number();
    }

    if(request->has_deck_detail_03()) {
        const auto &deck3 = request->deck_detail_03();
        user.replaceDeck(deck3.deck_type(), deck3.user_deck_number(), deck3.has_deck() ? &deck3.deck() : nullptr);
        deck3Number = deck3.user_deck_number();
    }

    user.updateTripleDeck(request->deck_type(), request->user_deck_number(), deck1Number, deck2Number, deck3Number);
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



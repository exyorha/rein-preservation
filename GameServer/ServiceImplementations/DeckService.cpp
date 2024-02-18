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


void DeckService::UpdateNameImpl(int64_t userId,
                                 const ::apb::api::deck::UpdateNameRequest* request,
                                 ::apb::api::deck::UpdateNameResponse* response) {

    auto updateName = db().prepare(R"SQL(
        INSERT INTO i_user_deck (
            user_id,
            deck_type,
            user_deck_number,
            name
        ) VALUES (
            ?, ?, ?, ?
        )
        ON CONFLICT (user_id, deck_type, user_deck_number) DO UPDATE SET
            name = excluded.name
    )SQL");
    updateName->bind(1, userId);
    updateName->bind(2, request->deck_type());
    updateName->bind(3, request->user_deck_number());
    updateName->bind(4, request->name());
    updateName->exec();
}

void DeckService::ReplaceDeckImpl(int64_t userId,
                                  const ::apb::api::deck::ReplaceDeckRequest* request,
                                  ::apb::api::deck::ReplaceDeckResponse* response) {

    replaceDeck(userId, request->deck_type(), request->user_deck_number(), request->has_deck() ? &request->deck() : nullptr);
}

void DeckService::RefreshDeckPowerImpl(int64_t userId,
                                       const ::apb::api::deck::RefreshDeckPowerRequest* request,
                                       ::apb::api::deck::RefreshDeckPowerResponse* response) {

    if(!request->has_deck_power())
        throw std::runtime_error("no deck power is specified in RefreshDeckPower");

    const auto &deckPower = request->deck_power();

    auto updateDeck = db().prepare(R"SQL(
        UPDATE i_user_deck SET
            power = ?
        WHERE
            user_id = ? AND
            deck_type = ? AND
            user_deck_number = ?
    )SQL");

    updateDeck->bind(1, deckPower.power());
    updateDeck->bind(2, userId);
    updateDeck->bind(3, request->deck_type());
    updateDeck->bind(4, request->user_deck_number());
    updateDeck->exec();

    if(deckPower.has_deck_character_power_01()) {
        updateDeckCharacterPower(userId, deckPower.deck_character_power_01());
    }

    if(deckPower.has_deck_character_power_02()) {
        updateDeckCharacterPower(userId, deckPower.deck_character_power_02());
    }

    if(deckPower.has_deck_character_power_03()) {
        updateDeckCharacterPower(userId, deckPower.deck_character_power_03());
    }
}

void DeckService::updateDeckCharacterPower(int64_t userId, const apb::api::deck::DeckCharacterPower &power) {
    auto updateCharacter = db().prepare(R"SQL(
        UPDATE i_user_deck_character SET
            power = ?
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
    )SQL");
    updateCharacter->bind(1, power.power());
    updateCharacter->bind(2, userId);
    updateCharacter->bind(3, power.user_deck_character_uuid());
    updateCharacter->exec();
}


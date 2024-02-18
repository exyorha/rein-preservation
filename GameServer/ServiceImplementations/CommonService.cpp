#include <ServiceImplementations/CommonService.h>

#include <DataModel/Database.h>
#include <DataModel/Sqlite/Statement.h>

#include <DataModel/DatabaseEnums.h>

#include <cstdio>

CommonService::CommonService(Database &db) : m_db(db) {

}

CommonService::~CommonService() = default;

::grpc::Status CommonService::guardedCall(const char *callName, const std::function<void()> &body) {
    try {

        body();

        return grpc::Status::OK;
    } catch(const std::exception &e) {

        fprintf(stderr, "An exception occurred while processing %s: %s\n", callName, e.what());

        return grpc::Status(grpc::StatusCode::INTERNAL, e.what());

    } catch(...) {
        fprintf(stderr, "An exception occurred while processing %s: unknown\n", callName);
        return grpc::Status(grpc::StatusCode::INTERNAL, "unknown");

    }
}

int64_t CommonService::authenticate(::grpc::ServerContext *context) {
    const auto &metadata = context->client_metadata();
    auto it = metadata.find("x-apb-session-key");
    if(it == metadata.end())
        throw std::runtime_error("'x-apb-session-key' not specified");

    const auto &session = it->second;

    auto statement = db().prepare("SELECT user_id FROM internal_sessions WHERE session_id = ? AND expire_datetime >= unixepoch('now')");
    statement->bind(1, std::string(session.begin(), session.end()));

    if(statement->step()) {
        return statement->columnInt64(0);
    }

    throw std::runtime_error("bad session key");
}

/*
 * This is needed for e.g. the mama menu tutorial to operate correctly
 */
void CommonService::buildDefaultDeckIfNoneExists(int64_t userId) {
    bool hasAnyDecks = false;

    auto checkDecks = db().prepare("SELECT 1 FROM i_user_deck WHERE user_id = ? AND deck_type = 1 LIMIT 1");
    checkDecks->bind(1, userId);
    while(checkDecks->step())
        hasAnyDecks = true;

    if(!hasAnyDecks) {
        std::string existingCostume;
        std::string existingWeapon;

        auto getFirstCostume = db().prepare("SELECT user_costume_uuid FROM i_user_costume WHERE user_id = ? ORDER BY acquisition_datetime LIMIT 1");
        getFirstCostume->bind(1, userId);
        while(getFirstCostume->step()) {
            existingCostume = getFirstCostume->columnText(0);
        }

        auto getFirstWeapon = db().prepare("SELECT user_weapon_uuid FROM i_user_weapon WHERE user_id = ? ORDER BY acquisition_datetime LIMIT 1");
        getFirstWeapon->bind(1, userId);
        while(getFirstWeapon->step()) {
            existingWeapon = getFirstWeapon->columnText(0);
        }

        if(!existingCostume.empty() && !existingWeapon.empty()) {
            printf("User %ld has no decks, but has at least one costume (%s) and a weapon (%s). Building the default deck\n",
                   userId, existingCostume.c_str(), existingWeapon.c_str());

            DeckInDatabaseRepresentation deckRepresentation;

            ::apb::api::deck::Deck deck;
            auto character = deck.mutable_character_01();
            character->set_user_costume_uuid(existingCostume);
            character->set_main_user_weapon_uuid(existingWeapon);
            replaceDeckCharacters(userId, deckRepresentation, &deck);

            auto insertDeck = db().prepare(R"SQL(
                INSERT INTO i_user_deck (
                    user_id,
                    deck_type,
                    user_deck_number,
                    user_deck_character_uuid01,
                    user_deck_character_uuid02,
                    user_deck_character_uuid03
                ) VALUES (
                    ?,
                    1,
                    1,
                    ?,
                    ?,
                    ?
                )
            )SQL");
            insertDeck->bind(1, userId);
            insertDeck->bind(2, deckRepresentation.characterUUIDs[0]);
            insertDeck->bind(3, deckRepresentation.characterUUIDs[1]);
            insertDeck->bind(4, deckRepresentation.characterUUIDs[2]);
            insertDeck->exec();
        }

    }
}


void CommonService::replaceDeckCharacters(int64_t userId, DeckInDatabaseRepresentation &storedDeck, const ::apb::api::deck::Deck *deck) {
    if(deck && deck->has_character_01()) {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[0], &deck->character_01());
    } else {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[0], nullptr);
    }

    if(deck && deck->has_character_02()) {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[1], &deck->character_02());
    } else {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[1], nullptr);
    }

    if(deck && deck->has_character_03()) {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[2], &deck->character_03());
    } else {
        replaceDeckCharacter(userId, storedDeck.characterUUIDs[2], nullptr);
    }
}

void CommonService::replaceDeckCharacter(int64_t userId,
                                       std::string &characterUUID,
                                       const ::apb::api::deck::DeckCharacter *character) {

    if(!character) {
        if(!characterUUID.empty()) {
            auto deleteExisting = db().prepare("DELETE FROM i_user_deck_character WHERE user_id = ? AND user_deck_character_uuid = ?");
            deleteExisting->bind(1, userId);
            deleteExisting->bind(2, characterUUID);
            deleteExisting->exec();

            characterUUID.clear();
        }

        return;
    }

    if(characterUUID.empty()) {
        auto insertNew = db().prepare(R"SQL(
            INSERT INTO i_user_deck_character (
                user_id,
                user_deck_character_uuid
            ) VALUES (
                ?,
                hex(randomblob(16))
            ) RETURNING user_deck_character_uuid
        )SQL");

        insertNew->bind(1, userId);
        while(insertNew->step()) {
            characterUUID = insertNew->columnText(0);
        }
    }

    auto updateCharacter = db().prepare(R"SQL(
        UPDATE i_user_deck_character SET
            user_costume_uuid = ?,
            main_user_weapon_uuid = ?,
            user_companion_uuid = ?,
            user_thought_uuid = ?
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
    )SQL");

    updateCharacter->bind(1, character->user_costume_uuid());
    updateCharacter->bind(2, character->main_user_weapon_uuid());
    updateCharacter->bind(3, character->user_companion_uuid());
    updateCharacter->bind(4, character->user_thought_uuid());
    updateCharacter->bind(5, userId);
    updateCharacter->bind(6, characterUUID);
    updateCharacter->exec();

    if(character->dressup_costume_id() == 0) {
        auto deleteDressupCostume = db().prepare(R"SQL(
            DELETE FROM i_user_deck_character_dressup_costume WHERE user_id = ? AND user_deck_character_uuid = ?
        )SQL");
        deleteDressupCostume->bind(1, userId);
        deleteDressupCostume->bind(2, characterUUID);
        deleteDressupCostume->exec();
    } else {
        auto insertDressupCostume = db().prepare(R"SQL(
            INSERT INTO i_user_deck_character_dressup_costume (
                user_id,
                user_deck_character_uuid,
                dressup_costume_id
            ) VALUES (
                dressup_costume_id = ?
            )
            ON CONFLICT (user_id, user_deck_character_uuid) DO UPDATE SET
                dressup_costume_id = excluded.dressup_costume_id
        )SQL");

        insertDressupCostume->bind(1, userId);
        insertDressupCostume->bind(2, characterUUID);
        insertDressupCostume->bind(3, character->dressup_costume_id());
        insertDressupCostume->exec();
    }

    auto deleteParts = db().prepare("DELETE FROM i_user_deck_parts_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteParts->bind(1, userId);
    deleteParts->bind(2, characterUUID);
    deleteParts->exec();

    int32_t sortOrder = 0;
    for(const auto &partUUID: character->user_parts_uuid()) {
        auto insertPart = db().prepare(R"SQL(
            INSERT INTO i_user_deck_parts_group (
                user_id,
                user_deck_character_uuid,
                user_parts_uuid,
                sort_order
            ) VALUES (
                ?,
                ?,
                ?,
                ?
            )
        )SQL");
        insertPart->bind(1, userId);
        insertPart->bind(2, characterUUID);
        insertPart->bind(3, partUUID);
        ++sortOrder;
        insertPart->bind(4, sortOrder);
        insertPart->exec();
    }

    auto deleteSubWeapons = db().prepare("DELETE FROM i_user_deck_sub_weapon_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteSubWeapons->bind(1, userId);
    deleteSubWeapons->bind(2, characterUUID);
    deleteSubWeapons->exec();

    sortOrder = 0;
    for(const auto &subWeaponUUID : character->sub_user_weapon_uuid()) {
        auto insertSubWeapon = db().prepare(R"SQL(
            INSERT INTO i_user_deck_sub_weapon_group (
                user_id,
                user_deck_character_uuid,
                user_weapon_uuid,
                sort_order
            ) VALUES (
                ?,
                ?,
                ?,
                ?
            )
        )SQL");

        insertSubWeapon->bind(1, userId);
        insertSubWeapon->bind(2, characterUUID);
        insertSubWeapon->bind(3, subWeaponUUID);
        ++sortOrder;
        insertSubWeapon->bind(4, sortOrder);
        insertSubWeapon->exec();
    }
}

void CommonService::givePossession(int64_t userId, int32_t possessionType, int32_t possessionId, int32_t count) {
    printf("QuestService: giving to %ld: possession type %d, id %d, count %d\n",
           userId, possessionType, possessionId, count);

    switch(static_cast<PossessionType>(possessionType)) {
        case PossessionType::COSTUME:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for COSTUME");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_costume (
                    user_id,
                    user_costume_uuid,
                    costume_id,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    hex(randomblob(16)),
                    ?,
                    current_net_timestamp()
                )
                RETURNING user_costume_uuid
            )SQL");

            query->bind(1, userId);
            query->bind(2, possessionId);
            std::string costumeUUID;
            while(query->step()) {
                costumeUUID = query->columnText(0);
            }

            auto activeSkill = db().prepare(R"SQL(
                INSERT INTO i_user_costume_active_skill (
                    user_id,
                    user_costume_uuid,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    current_net_timestamp()
                )
            )SQL");

            activeSkill->bind(1, userId);
            activeSkill->bind(2, costumeUUID);
            activeSkill->exec();

            auto addCharacter = db().prepare(R"SQL(
                INSERT INTO i_user_character (user_id, character_id)
                SELECT
                    ? AS user_id,
                    character_id
                FROM
                    m_costume
                WHERE
                    costume_id = ?
                ON CONFLICT (user_id, character_id) DO NOTHING
            )SQL");
            addCharacter->bind(1, userId);
            addCharacter->bind(2, possessionId);
            addCharacter->exec();
        }
        break;

        case PossessionType::WEAPON:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for WEAPON");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_weapon (
                    user_id,
                    user_weapon_uuid,
                    weapon_id,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    hex(randomblob(16)),
                    ?,
                    current_net_timestamp()
                )
                RETURNING user_weapon_uuid
            )SQL");

            query->bind(1, userId);
            query->bind(2, possessionId);
            std::string weaponUuid;
            if(query->step())
                weaponUuid = query->columnText(0);

            auto note = db().prepare(R"SQL(
                INSERT INTO i_user_weapon_note (
                    user_id,
                    weapon_id,
                    first_acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    current_net_timestamp()
                )
                ON CONFLICT DO NOTHING
            )SQL");

            note->bind(1, userId);
            note->bind(2, possessionId);
            note->exec();

            auto story = db().prepare(R"SQL(
                INSERT INTO i_user_weapon_story (
                    user_id,
                    weapon_id
                ) VALUES (
                    ?,
                    ?
                )
                ON CONFLICT DO NOTHING
            )SQL");

            story->bind(1, userId);
            story->bind(2, possessionId);
            story->exec();

            auto skill = db().prepare(R"SQL(
                INSERT INTO i_user_weapon_skill (
                    user_id,
                    user_weapon_uuid,
                    slot_number
                ) VALUES (
                    ?,
                    ?,
                    1
                )
            )SQL");

            skill->bind(1, userId);
            skill->bind(2, weaponUuid);
            skill->exec();

            auto ability = db().prepare(R"SQL(
                INSERT INTO i_user_weapon_ability (
                    user_id,
                    user_weapon_uuid,
                    slot_number
                ) VALUES (
                    ?,
                    ?,
                    1
                )
            )SQL");

            ability->bind(1, userId);
            ability->bind(2, weaponUuid);
            ability->exec();
        }
        break;


        case PossessionType::PARTS:
        {
            return;
            if(count != 1)
                throw std::runtime_error("Unexpected count value for WEAPON");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_parts (
                    user_id,
                    user_parts_uuid,
                    parts_id,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    hex(randomblob(16)),
                    ?,
                    current_net_timestamp()
                )
                ON CONFLICT (user_id, parts_id) DO NOTHING
            )SQL");

            query->bind(1, userId);
            query->bind(2, possessionId);
            query->exec();
        }
        break;

        case PossessionType::FREE_GEM:
        {
            if(count <= 0)
                throw std::runtime_error("Unexpected count value for FREE_GEM");

            auto query = db().prepare("UPDATE i_user_gem SET free_gem = free_gem + ? WHERE user_id = ?");
            query->bind(1, count);
            query->bind(2, userId);
            query->exec();
        }
        break;

        case PossessionType::IMPORTANT_ITEM:
        {
            if(count <= 0)
                throw std::runtime_error("Unexpected count value for IMPORTANT_ITEM");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_important_item (
                    user_id,
                    important_item_id,
                    count,
                    first_acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    ?,
                    current_net_timestamp()
                )
                ON CONFLICT (user_id, important_item_id) DO UPDATE SET count = count + excluded.count
            )SQL");
            query->bind(1, userId);
            query->bind(2, possessionId);
            query->bind(3, count);
            query->exec();
        }
        break;

        default:
            throw std::runtime_error("unsupported possession type " + std::to_string(possessionType));
    }

    buildDefaultDeckIfNoneExists(userId);
}

void CommonService::replaceDeck(int64_t userId, int32_t deckType, int32_t userDeckNumber, const apb::api::deck::Deck *deckDefinition) {

    DeckInDatabaseRepresentation deck;

    auto getExistingDeck = db().prepare(R"SQL(
        SELECT
            user_deck_character_uuid01,
            user_deck_character_uuid02,
            user_deck_character_uuid03
        FROM
            i_user_deck
        WHERE
            user_id = ? AND
            deck_type = ? AND
            user_deck_number = ?
    )SQL");

    getExistingDeck->bind(1, userId);
    getExistingDeck->bind(2, deckType);
    getExistingDeck->bind(3, userDeckNumber);
    if(getExistingDeck->step()) {

        deck.characterUUIDs[0] = getExistingDeck->columnText(0);
        deck.characterUUIDs[1] = getExistingDeck->columnText(1);
        deck.characterUUIDs[2] = getExistingDeck->columnText(2);
    }

    replaceDeckCharacters(userId, deck, deckDefinition);

    auto updateDeck = db().prepare(R"SQL(
        INSERT INTO i_user_deck (
            user_id,
            deck_type,
            user_deck_number,
            user_deck_character_uuid01,
            user_deck_character_uuid02,
            user_deck_character_uuid03
        ) VALUES (
            ?, ?, ?, ?, ?, ?
        )
        ON CONFLICT (user_id, deck_type, user_deck_number) DO UPDATE SET
            user_deck_character_uuid01 = excluded.user_deck_character_uuid01,
            user_deck_character_uuid02 = excluded.user_deck_character_uuid02,
            user_deck_character_uuid03 = excluded.user_deck_character_uuid03
    )SQL");

    updateDeck->bind(1, userId);
    updateDeck->bind(2, deckType);
    updateDeck->bind(3, userDeckNumber);
    updateDeck->bind(4, deck.characterUUIDs[0]);
    updateDeck->bind(5, deck.characterUUIDs[1]);
    updateDeck->bind(6, deck.characterUUIDs[2]);
    updateDeck->exec();
}

#include <DataModel/UserContext.h>
#include <DataModel/DatabaseEnums.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/Sqlite/Transaction.h>

#include <service/DeckService.pb.h>
#include <service/QuestService.pb.h>

#include <string>

UserContext::UserContext(Database &db, int64_t userId) : DatabaseContext(db), m_userId(userId), m_log(makeFacilityString(userId)) {

}

UserContext::UserContext(DatabaseContext &context, int64_t userId) : UserContext(context.dataModel(), userId) {

}

UserContext::~UserContext() = default;

std::string UserContext::makeFacilityString(int64_t userId) {
    std::stringstream facility;
    facility << "UserContext(" << userId << ")";
    return facility.str();
}

/*
 * This is needed for e.g. the mama menu tutorial to operate correctly
 */
void UserContext::buildDefaultDeckIfNoneExists() {
    bool hasAnyDecks = false;

    auto checkDecks = db().prepare("SELECT 1 FROM i_user_deck WHERE user_id = ? AND deck_type = 1 LIMIT 1");
    checkDecks->bind(1, m_userId);
    while(checkDecks->step())
        hasAnyDecks = true;

    if(!hasAnyDecks) {
        std::string existingCostume;
        std::string existingWeapon;

        auto getFirstCostume = db().prepare("SELECT user_costume_uuid FROM i_user_costume WHERE user_id = ? ORDER BY acquisition_datetime LIMIT 1");
        getFirstCostume->bind(1, m_userId);
        while(getFirstCostume->step()) {
            existingCostume = getFirstCostume->columnText(0);
        }

        auto getFirstWeapon = db().prepare("SELECT user_weapon_uuid FROM i_user_weapon WHERE user_id = ? ORDER BY acquisition_datetime LIMIT 1");
        getFirstWeapon->bind(1, m_userId);
        while(getFirstWeapon->step()) {
            existingWeapon = getFirstWeapon->columnText(0);
        }

        if(!existingCostume.empty() && !existingWeapon.empty()) {
            m_log.info("user has no decks, but has at least one costume (%s) and a weapon (%s). Building the default deck\n",
                       existingCostume.c_str(), existingWeapon.c_str());

            DeckInDatabaseRepresentation deckRepresentation;

            ::apb::api::deck::Deck deck;
            auto character = deck.mutable_character_01();
            character->set_user_costume_uuid(existingCostume);
            character->set_main_user_weapon_uuid(existingWeapon);
            replaceDeckCharacters(deckRepresentation, &deck);

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
            insertDeck->bind(1, m_userId);
            insertDeck->bind(2, deckRepresentation.characterUUIDs[0]);
            insertDeck->bind(3, deckRepresentation.characterUUIDs[1]);
            insertDeck->bind(4, deckRepresentation.characterUUIDs[2]);
            insertDeck->exec();
        }

    }
}

void UserContext::replaceDeckCharacters(DeckInDatabaseRepresentation &storedDeck, const ::apb::api::deck::Deck *deck) {
    if(deck && deck->has_character_01()) {
        replaceDeckCharacter(storedDeck.characterUUIDs[0], &deck->character_01());
    } else {
        replaceDeckCharacter(storedDeck.characterUUIDs[0], nullptr);
    }

    if(deck && deck->has_character_02()) {
        replaceDeckCharacter(storedDeck.characterUUIDs[1], &deck->character_02());
    } else {
        replaceDeckCharacter(storedDeck.characterUUIDs[1], nullptr);
    }

    if(deck && deck->has_character_03()) {
        replaceDeckCharacter(storedDeck.characterUUIDs[2], &deck->character_03());
    } else {
        replaceDeckCharacter(storedDeck.characterUUIDs[2], nullptr);
    }
}


void UserContext::replaceDeckCharacter(std::string &characterUUID,
                                       const ::apb::api::deck::DeckCharacter *character) {

    if(!character) {
        if(!characterUUID.empty()) {
            auto deleteExisting = db().prepare("DELETE FROM i_user_deck_character WHERE user_id = ? AND user_deck_character_uuid = ?");
            deleteExisting->bind(1, m_userId);
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

        insertNew->bind(1, m_userId);
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
    updateCharacter->bind(5, m_userId);
    updateCharacter->bind(6, characterUUID);
    updateCharacter->exec();

    if(character->dressup_costume_id() == 0) {
        auto deleteDressupCostume = db().prepare(R"SQL(
            DELETE FROM i_user_deck_character_dressup_costume WHERE user_id = ? AND user_deck_character_uuid = ?
        )SQL");
        deleteDressupCostume->bind(1, m_userId);
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

        insertDressupCostume->bind(1, m_userId);
        insertDressupCostume->bind(2, characterUUID);
        insertDressupCostume->bind(3, character->dressup_costume_id());
        insertDressupCostume->exec();
    }

    auto deleteParts = db().prepare("DELETE FROM i_user_deck_parts_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteParts->bind(1, m_userId);
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
        insertPart->bind(1, m_userId);
        insertPart->bind(2, characterUUID);
        insertPart->bind(3, partUUID);
        ++sortOrder;
        insertPart->bind(4, sortOrder);
        insertPart->exec();
    }

    auto deleteSubWeapons = db().prepare("DELETE FROM i_user_deck_sub_weapon_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteSubWeapons->bind(1, m_userId);
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

        insertSubWeapon->bind(1, m_userId);
        insertSubWeapon->bind(2, characterUUID);
        insertSubWeapon->bind(3, subWeaponUUID);
        ++sortOrder;
        insertSubWeapon->bind(4, sortOrder);
        insertSubWeapon->exec();
    }
}


void UserContext::givePossession(int32_t possessionType, int32_t possessionId, int32_t count,
                                 google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards) {
    m_log.debug("giving possession type %d, id %d, count %d\n",
            possessionType, possessionId, count);

    if(addToQuestRewards) {
        auto reward = addToQuestRewards->Add();
        reward->set_possession_type(possessionType);
        reward->set_possession_id(possessionId);
        reward->set_count(count);
    }

    switch(static_cast<PossessionType>(possessionType)) {
        case PossessionType::COSTUME:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for COSTUME");

            giveUserCostume(possessionId);
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

            query->bind(1, m_userId);
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

            note->bind(1, m_userId);
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

            story->bind(1, m_userId);
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
                    ?
                )
            )SQL");

            skill->bind(1, m_userId);
            skill->bind(2, weaponUuid);

            /*
             * All weapons get two skill slots.
             */
            for(int slot: { 1, 2 }) {
                skill->bind(3, slot);
                skill->exec();
                skill->reset();
            }

            /*
             * Unevolved weapons only get one ability slot.
             */
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

            ability->bind(1, m_userId);
            ability->bind(2, weaponUuid);
            ability->exec();

            updateWeaponUnlockedStory(possessionId);
        }
        break;

        case PossessionType::COMPANION:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for COMPANION");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_companion (
                    user_id,
                    user_companion_uuid,
                    companion_id,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    hex(randomblob(16)),
                    ?,
                    current_net_timestamp()
                )
            )SQL");

            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->exec();
        }
        break;

        case PossessionType::PARTS:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for PARTS");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_parts (
                    user_id,
                    user_parts_uuid,
                    parts_id,
                    acquisition_datetime,
                    parts_status_main_id
                ) VALUES (
                    ?,
                    hex(randomblob(16)),
                    ?,
                    current_net_timestamp(),
                    1 -- TODO: XXX: 'parts_status_main_id' is supposed to be determined by a lottery, however, the data for it
                      -- appears to be absent. For now, we always set a valid key here to avoid crashing the client UI.
                )
            )SQL");

            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->exec();
        }
        break;

        case PossessionType::MATERIAL:
        {
            if(count <= 0)
                throw std::runtime_error("Unexpected count value for MATERIAL");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_material (
                    user_id,
                    material_id,
                    count,
                    first_acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    ?,
                    current_net_timestamp()
                )
                ON CONFLICT (user_id, material_id) DO UPDATE SET count = count + excluded.count
            )SQL");
            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->bind(3, count);
            query->exec();
        }
        break;

        case PossessionType::CONSUMABLE_ITEM:
        {
            if(count <= 0)
                throw std::runtime_error("Unexpected count value for CONSUMABLE_ITEM");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_consumable_item (
                    user_id,
                    consumable_item_id,
                    count,
                    first_acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    ?,
                    current_net_timestamp()
                )
                ON CONFLICT (user_id, consumable_item_id) DO UPDATE SET count = count + excluded.count
            )SQL");
            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->bind(3, count);
            query->exec();
        }
        break;

        case PossessionType::COSTUME_ENHANCED:
        {

            if(count != 1)
                throw std::runtime_error("Unexpected count value for COSTUME_ENHANCED");

            giveUserCostumeEnhanced(possessionId);

        }
        break;

        case PossessionType::FREE_GEM:
        {
            if(count <= 0)
                throw std::runtime_error("Unexpected count value for FREE_GEM");

            auto query = db().prepare("UPDATE i_user_gem SET free_gem = free_gem + ? WHERE user_id = ?");
            query->bind(1, count);
            query->bind(2, m_userId);
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
            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->bind(3, count);
            query->exec();
        }
        break;

        default:
            throw std::runtime_error("unsupported possession type " + std::to_string(possessionType));
    }

    buildDefaultDeckIfNoneExists();
}

void UserContext::giveUserCostumeEnhanced(int32_t costumeEnhancedId) {
    auto query = db().prepare(R"SQL(
        SELECT
            costume_id,
            limit_break_count,
            level,
            active_skill_level,
            awaken_count
        FROM
            m_costume_enhanced
        WHERE
            costume_enhanced_id = ?
    )SQL");
    query->bind(1, costumeEnhancedId);
    if(!query->step())
        throw std::runtime_error("no such enhanced costume");

    auto costumeId = query->columnInt(0);
    auto limitBreakCount = query->columnInt(1);
    auto level = query->columnInt(2);
    auto activeSkillLevel = query->columnInt(3);
    auto awakenCount = query->columnInt(4);
    query->reset();

    giveUserCostume(costumeId, limitBreakCount, level, activeSkillLevel, awakenCount);
}

void UserContext::giveUserCostume(
    int32_t costumeId,
    int32_t limitBreakCount,
    int32_t level,
    int32_t activeSkillLevel,
    int32_t awakenCount) {

    auto getLevelMap = db().prepare(R"SQL(
        SELECT
            required_exp_for_level_up_numerical_parameter_map_id
        FROM
            m_costume,
            m_costume_rarity ON m_costume_rarity.rarity_type = m_costume.rarity_type
        WHERE
            costume_id = ?
    )SQL");

    getLevelMap->bind(1, costumeId);
    if(!getLevelMap->step())
        throw std::runtime_error("no such costume");
    auto expFunction = getLevelMap->columnInt(0);
    getLevelMap->reset();

    auto exp = getNumericalParameterMapValue(expFunction, level);
    if(!exp.has_value())
        throw std::runtime_error("costume level is not in the exp map");

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_costume (
            user_id,
            user_costume_uuid,
            costume_id,
            limit_break_count,
            level,
            exp,
            acquisition_datetime,
            awaken_count
        ) VALUES (
            ?,
            hex(randomblob(16)),
            ?,
            ?,
            ?,
            ?,
            current_net_timestamp(),
            ?
        )
        RETURNING user_costume_uuid
    )SQL");

    query->bind(1, m_userId);
    query->bind(2, costumeId);
    query->bind(3, limitBreakCount);
    query->bind(4, level);
    query->bind(5, *exp);
    query->bind(6, awakenCount);

    std::string costumeUUID;
    while(query->step()) {
        costumeUUID = query->columnText(0);
    }

    auto activeSkill = db().prepare(R"SQL(
        INSERT INTO i_user_costume_active_skill (
            user_id,
            user_costume_uuid,
            level,
            acquisition_datetime
        ) VALUES (
            ?,
            ?,
            ?,
            current_net_timestamp()
        )
    )SQL");

    activeSkill->bind(1, m_userId);
    activeSkill->bind(2, costumeUUID);
    activeSkill->bind(3, activeSkillLevel);
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
    addCharacter->bind(1, m_userId);
    addCharacter->bind(2, costumeId);
    addCharacter->exec();
}


void UserContext::readDeck(int32_t deckType, int32_t userDeckNumber, apb::api::deck::Deck *deckDefinition) {
    DeckInDatabaseRepresentation deck;

    readDeckRepresentation(deckType, userDeckNumber, deck);

    if(!deck.characterUUIDs[0].empty()) {
        readDeckCharacterRepresentation(deck.characterUUIDs[0], deckDefinition->mutable_character_01());
    }

    if(!deck.characterUUIDs[1].empty()) {
        readDeckCharacterRepresentation(deck.characterUUIDs[1], deckDefinition->mutable_character_02());
    }

    if(!deck.characterUUIDs[2].empty()) {
        readDeckCharacterRepresentation(deck.characterUUIDs[2], deckDefinition->mutable_character_03());
    }
}

void UserContext::readDeckRepresentation(int32_t deckType, int32_t userDeckNumber, DeckInDatabaseRepresentation &deck) {

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

    getExistingDeck->bind(1, m_userId);
    getExistingDeck->bind(2, deckType);
    getExistingDeck->bind(3, userDeckNumber);
    if(getExistingDeck->step()) {

        deck.characterUUIDs[0] = getExistingDeck->columnText(0);
        deck.characterUUIDs[1] = getExistingDeck->columnText(1);
        deck.characterUUIDs[2] = getExistingDeck->columnText(2);
    }
}

void UserContext::readDeckCharacterRepresentation(const std::string_view &deckCharacterUUID, apb::api::deck::DeckCharacter *character) {
    auto queryCharacter = db().prepare(R"SQL(
        SELECT
            user_costume_uuid,
            main_user_weapon_uuid,
            user_companion_uuid,
            user_thought_uuid
        FROM i_user_deck_character
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
    )SQL");
    queryCharacter->bind(1, m_userId);
    queryCharacter->bind(2, deckCharacterUUID);
    if(queryCharacter->step()) {
        character->set_user_costume_uuid(queryCharacter->columnText(0));
        character->set_main_user_weapon_uuid(queryCharacter->columnText(1));
        character->set_user_companion_uuid(queryCharacter->columnText(2));
        character->set_user_thought_uuid(queryCharacter->columnText(3));
    }
    queryCharacter->reset();

    auto queryDressupCostume = db().prepare(R"SQL(
        SELECT
            dressup_costume_id
        FROM i_user_deck_character_dressup_costume
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
    )SQL");
    queryDressupCostume->bind(1, m_userId);
    queryDressupCostume->bind(2, deckCharacterUUID);
    if(queryDressupCostume->step()) {
        character->set_dressup_costume_id(queryDressupCostume->columnInt(0));
    }
    queryDressupCostume->reset();

    auto queryParts = db().prepare(R"SQL(
        SELECT
            user_parts_uuid
        FROM i_user_deck_parts_group
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
        ORDER BY sort_order
    )SQL");
    queryParts->bind(1, m_userId);
    queryParts->bind(2, deckCharacterUUID);
    while(queryParts->step()) {
        character->add_user_parts_uuid(queryParts->columnText(0));
    }
    queryParts->reset();

    auto querySubWeapons = db().prepare(R"SQL(
        SELECT
            user_weapon_uuid
        FROM i_user_deck_sub_weapon_group
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
        ORDER BY sort_order
    )SQL");
    querySubWeapons->bind(1, m_userId);
    querySubWeapons->bind(2, deckCharacterUUID);
    while(querySubWeapons->step()) {
        character->add_sub_user_weapon_uuid(querySubWeapons->columnText(0));
    }
    querySubWeapons->reset();
}

void UserContext::replaceDeck(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::Deck *deckDefinition) {

    DeckInDatabaseRepresentation deck;
    readDeckRepresentation(deckType, userDeckNumber, deck);

    replaceDeckCharacters(deck, deckDefinition);

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

    updateDeck->bind(1, m_userId);
    updateDeck->bind(2, deckType);
    updateDeck->bind(3, userDeckNumber);
    updateDeck->bind(4, deck.characterUUIDs[0]);
    updateDeck->bind(5, deck.characterUUIDs[1]);
    updateDeck->bind(6, deck.characterUUIDs[2]);
    updateDeck->exec();
}

void UserContext::deleteDeck(int32_t deckType, int32_t userDeckNumber) {
    DeckInDatabaseRepresentation deck;

    readDeckRepresentation(deckType, userDeckNumber, deck);

    if(!deck.characterUUIDs[0].empty()) {
        deleteDeckCharacter(deck.characterUUIDs[0]);
    }

    if(!deck.characterUUIDs[1].empty()) {
        deleteDeckCharacter(deck.characterUUIDs[1]);
    }

    if(!deck.characterUUIDs[2].empty()) {
        deleteDeckCharacter(deck.characterUUIDs[2]);
    }

    auto deleteDeck = db().prepare("DELETE FROM i_user_deck WHERE user_id = ? AND deck_type = ? AND user_deck_number = ?");
    deleteDeck->bind(1, m_userId);
    deleteDeck->bind(2, deckType);
    deleteDeck->bind(3, userDeckNumber);
    deleteDeck->exec();
}

void UserContext::deleteDeckCharacter(const std::string_view &deckCharacterUUID) {
    auto deleteSubWeapons = db().prepare("DELETE FROM i_user_deck_sub_weapon_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteSubWeapons->bind(1, m_userId);
    deleteSubWeapons->bind(2, deckCharacterUUID);
    deleteSubWeapons->exec();

    auto deleteParts = db().prepare("DELETE FROM i_user_deck_parts_group WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteParts->bind(1, m_userId);
    deleteParts->bind(2, deckCharacterUUID);
    deleteParts->exec();

    auto deleteDressupCostume = db().prepare("DELETE FROM i_user_deck_character_dressup_costume WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteDressupCostume->bind(1, m_userId);
    deleteDressupCostume->bind(2, deckCharacterUUID);
    deleteDressupCostume->exec();

    auto deleteCharacter = db().prepare("DELETE FROM i_user_deck_character WHERE user_id = ? AND user_deck_character_uuid = ?");
    deleteCharacter->bind(1, m_userId);
    deleteCharacter->bind(2, deckCharacterUUID);
    deleteCharacter->exec();
}

void UserContext::giveUserExperience(int32_t experience) {
    if(experience < 0)
        throw std::logic_error("experience cannot be negative");

    m_log.debug("giving %d experience\n", experience);

    auto query = db().prepare("UPDATE i_user_status SET exp = exp + ? WHERE user_id = ? RETURNING exp, level");
    int32_t currentTotalExp = 0, currentLevel = 0;

    query->bind(1, experience);
    query->bind(2, m_userId);
    while(query->step()) {
        currentTotalExp = query->columnInt(0);
        currentLevel = query->columnInt(1);
    }

    auto newLevel = evaluateNumericalParameterMap(userLevelExpNumericalParameterMapID(), currentTotalExp);
    // TODO: clamp by max level (if one exists)
    if(newLevel.has_value() && *newLevel != currentLevel) {
        m_log.debug("user has leveled up: %d -> %d\n", currentLevel, *newLevel);
        auto updateLevel = db().prepare("UPDATE i_user_status SET level = ? WHERE user_id = ?");
        updateLevel->bind(1, *newLevel);
        updateLevel->bind(2, m_userId);
        updateLevel->exec();
    }
}



void UserContext::giveUserDeckExperience(
    int32_t deckType,
    int32_t userDeckNumber,
    int32_t characterExperience,
    int32_t costumeExperience) {

    auto query = db().prepare(R"SQL(
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
    query->bind(1, m_userId);
    query->bind(2, deckType);
    query->bind(3, userDeckNumber);

    while(query->step()) {
        std::string uuid1 = query->columnText(0);
        std::string uuid2 = query->columnText(1);
        std::string uuid3 = query->columnText(2);

        if(!uuid1.empty()) {
            giveUserDeckCharacterExperience(uuid1, characterExperience, costumeExperience);
        }

        if(!uuid2.empty()) {
            giveUserDeckCharacterExperience(uuid2, characterExperience, costumeExperience);
        }

        if(!uuid3.empty()) {
            giveUserDeckCharacterExperience(uuid3, characterExperience, costumeExperience);
        }
    }
}

void UserContext::giveUserDeckCharacterExperience(
    const std::string &userDeckCharacterUuid,
    int32_t characterExperience,
    int32_t costumeExperience) {

    std::string costumeUuid;
    std::string weaponUuid;

    auto query = db().prepare(R"SQL(
        SELECT user_costume_uuid, main_user_weapon_uuid FROM i_user_deck_character WHERE user_id = ? AND user_deck_character_uuid = ?
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, userDeckCharacterUuid);
    if(query->step())  {
        costumeUuid = query->columnText(0);
        weaponUuid = query->columnText(1);
    }

    if(!costumeUuid.empty()) {
        giveUserCostumeExperience(costumeUuid, characterExperience, costumeExperience);
    }

    if(!weaponUuid.empty()) {
        giveUserWeaponExperience(weaponUuid, characterExperience, costumeExperience);
    }

    auto subWeapons = db().prepare(R"SQL(
        SELECT user_weapon_uuid FROM i_user_deck_sub_weapon_group WHERE user_id = ? AND user_deck_character_uuid = ?
    )SQL");

    subWeapons->bind(1, m_userId);
    subWeapons->bind(2, userDeckCharacterUuid);
    while(subWeapons->step()) {
        std::string subWeapon = subWeapons->columnText(0);
        if(!subWeapon.empty()) {
            giveUserWeaponExperience(subWeapon, characterExperience, costumeExperience);
        }
    }
}

void UserContext::giveUserCostumeExperience(const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience) {
    m_log.debug("giving costume %s %d character experience and %d costume experience",
        userCostumeUuid.c_str(), characterExperience, costumeExperience);

    auto updateExperience = db().prepare(R"SQL(
        UPDATE i_user_costume SET exp = exp + ?
        WHERE user_id = ? AND user_costume_uuid = ?
        RETURNING costume_id, level, exp, limit_break_count
    )SQL");
    updateExperience->bind(1, costumeExperience);
    updateExperience->bind(2, m_userId);
    updateExperience->bind(3, userCostumeUuid);
    if(!updateExperience->step())
        throw std::logic_error("no such costume");

    auto costumeId = updateExperience->columnInt(0);
    auto currentLevel = updateExperience->columnInt(1);
    auto newExperience = updateExperience->columnInt(2);
    auto limitBreakCount = updateExperience->columnInt(3);

    updateExperience->step();

    auto queryCostumeExperienceSetup = db().prepare(R"SQL(
        SELECT
            required_exp_for_level_up_numerical_parameter_map_id,
            costume_level_bonus_id,
            character_id,
            max_level_numerical_function_id
        FROM m_costume
        LEFT JOIN m_costume_rarity ON m_costume_rarity.rarity_type = m_costume.rarity_type
        WHERE
            costume_id = ?
    )SQL");
    queryCostumeExperienceSetup->bind(1, costumeId);
    if(!queryCostumeExperienceSetup->step())
        throw std::runtime_error("the costume setup was not found");

    int32_t costumeExpLevelMap = queryCostumeExperienceSetup->columnInt(0);
    int32_t costumeLevelBonusId = queryCostumeExperienceSetup->columnInt(1);
    int32_t characterId = queryCostumeExperienceSetup->columnInt(2);
    int32_t maxLevelFunction = queryCostumeExperienceSetup->columnInt(3);
    queryCostumeExperienceSetup->step();

    auto newLevel = evaluateNumericalParameterMap(costumeExpLevelMap, newExperience);

    if(newLevel.has_value()) {
        auto maxLevel = evaluateNumericalFunction(maxLevelFunction, limitBreakCount);

        auto getAddedMaxLevelsFromCharacterRebirths = db().prepare(R"SQL(
            SELECT
                COALESCE(SUM(costume_level_limit_up), 0)
            FROM
                m_character_rebirth,
                m_character_rebirth_step_group USING (character_rebirth_step_group_id) LEFT JOIN
                i_user_character_rebirth USING (character_id)
            WHERE
                user_id = ? AND
                character_id = ? AND
                COALESCE(rebirth_count, 0) > before_rebirth_count
        )SQL");

        getAddedMaxLevelsFromCharacterRebirths->bind(1, m_userId);
        getAddedMaxLevelsFromCharacterRebirths->bind(2, characterId);

        if(getAddedMaxLevelsFromCharacterRebirths->step()) {
            maxLevel += getAddedMaxLevelsFromCharacterRebirths->columnInt(0);
        }

        newLevel = std::min<int32_t>(*newLevel, maxLevel);
    }

    if(newLevel.has_value() && *newLevel != currentLevel) {
        m_log.debug("costume %s has leveled up: %d -> %d",
           userCostumeUuid.c_str(), currentLevel, *newLevel);

        auto updateLevel = db().prepare("UPDATE i_user_costume SET level = ? WHERE user_id = ? AND user_costume_uuid = ?");
        updateLevel->bind(1, *newLevel);
        updateLevel->bind(2, m_userId);
        updateLevel->bind(3, userCostumeUuid);
        updateLevel->exec();

        /*
         * Query if there's a new level bonus.
         */
        auto queryLevelBonus = db().prepare(R"SQL(
            SELECT level FROM m_costume_level_bonus WHERE costume_level_bonus_id = ? AND level <= ? ORDER BY level DESC LIMIT 1
        )SQL");
        int32_t highestLevelBonusAvailable = 0;
        queryLevelBonus->bind(1, costumeLevelBonusId);
        queryLevelBonus->bind(2, *newLevel);
        if(queryLevelBonus->step()) {
            highestLevelBonusAvailable = queryLevelBonus->columnInt(0);
        }

        if(highestLevelBonusAvailable != 0) {
            /*
             * Update the level bonus status record.
             */

            auto updateLevelBonus = db().prepare(R"SQL(
                INSERT INTO i_user_costume_level_bonus_release_status (
                    user_id,
                    costume_id,
                    last_released_bonus_level
                ) VALUES (
                    ?,
                    ?,
                    ?
                )
                ON CONFLICT (user_id, costume_id) DO UPDATE SET
                    last_released_bonus_level = MAX(last_released_bonus_level, excluded.last_released_bonus_level)
            )SQL");
            updateLevelBonus->bind(1, m_userId);
            updateLevelBonus->bind(2, costumeId);
            updateLevelBonus->bind(3, highestLevelBonusAvailable);
            updateLevelBonus->exec();
        }

        reevaluateCharacterCostumeLevelBonuses(characterId);
    }

    if(characterExperience != 0) {
        giveUserCharacterExperience(characterId, characterExperience);
    }
}


void UserContext::giveUserCharacterExperience(int32_t characterId, int32_t characterExperience) {
    m_log.debug("Giving character %d %d character experience", characterId, characterExperience);

    auto updateExperience = db().prepare(R"SQL(
        UPDATE i_user_character SET exp = exp + ?
        WHERE user_id = ? AND character_id = ?
        RETURNING level, exp
    )SQL");
    updateExperience->bind(1, characterExperience);
    updateExperience->bind(2, m_userId);
    updateExperience->bind(3, characterId);
    if(!updateExperience->step())
        throw std::logic_error("no such character");

    auto currentLevel = updateExperience->columnInt(0);
    auto newExperience = updateExperience->columnInt(1);

    updateExperience->step();

    auto queryCharacterExperienceSetup = db().prepare(R"SQL(
        SELECT
            required_exp_for_level_up_numerical_parameter_map_id,
            max_level_numerical_function_id
        FROM m_character
        WHERE
            character_id = ?
    )SQL");
    queryCharacterExperienceSetup->bind(1, characterId);
    if(!queryCharacterExperienceSetup->step())
        throw std::runtime_error("the character setup was not found");

    int32_t characterExpLevelMap = queryCharacterExperienceSetup->columnInt(0);
    int32_t maxLevelNumericalFunction = queryCharacterExperienceSetup->columnInt(1);
    queryCharacterExperienceSetup->step();

    auto newLevel = evaluateNumericalParameterMap(characterExpLevelMap, newExperience);
    if(newLevel.has_value()) {
        auto maxLevel = evaluateNumericalFunction(maxLevelNumericalFunction, 0); /* it appears that the input is always 0. */
        newLevel = std::min(*newLevel, maxLevel);
    }

    if(newLevel.has_value() && *newLevel != currentLevel) {
        m_log.debug("character %d has leveled up: %d -> %d\n", characterId, currentLevel, *newLevel);

        auto updateLevel = db().prepare("UPDATE i_user_character SET level = ? WHERE user_id = ? AND character_id = ?");
        updateLevel->bind(1, *newLevel);
        updateLevel->bind(2, m_userId);
        updateLevel->bind(3, characterId);
        updateLevel->exec();

        // any level bonuses?

    }
}

void UserContext::giveUserWeaponExperience(const std::string &userWeaponUuid, int32_t characterExperience, int32_t costumeExperience) {
    m_log.debug("Giving weapon %s %d character experience and %d costume experience", userWeaponUuid.c_str(), characterExperience, costumeExperience);

    auto updateExperience = db().prepare(R"SQL(
        UPDATE i_user_weapon SET exp = exp + ?
        WHERE user_id = ? AND user_weapon_uuid = ?
        RETURNING weapon_id, level, exp, limit_break_count
    )SQL");
    updateExperience->bind(1, costumeExperience);
    updateExperience->bind(2, m_userId);
    updateExperience->bind(3, userWeaponUuid);
    if(!updateExperience->step())
        throw std::logic_error("no such weapon");

    auto weaponId = updateExperience->columnInt(0);
    auto currentLevel = updateExperience->columnInt(1);
    auto newExperience = updateExperience->columnInt(2);
    auto limitBreakCount = updateExperience->columnInt(3);

    updateExperience->step();

    auto queryWeaponExperienceSetup = db().prepare(R"SQL(
        SELECT
            required_exp_for_level_up_numerical_parameter_map_id,
            max_level_numerical_function_id
        FROM m_weapon
        LEFT JOIN m_weapon_rarity ON m_weapon_rarity.rarity_type = m_weapon.rarity_type
        WHERE
            weapon_id = ?
    )SQL");
    queryWeaponExperienceSetup->bind(1, weaponId);
    if(!queryWeaponExperienceSetup->step())
        throw std::runtime_error("the weapon setup was not found");

    int32_t weaponExpLevelMap = queryWeaponExperienceSetup->columnInt(0);
    int32_t maxLevelNumericalFunctionId = queryWeaponExperienceSetup->columnInt(1);
    queryWeaponExperienceSetup->step();

    auto newLevel = evaluateNumericalParameterMap(weaponExpLevelMap, newExperience);
    if(newLevel.has_value()) {
        /*
         * TODO: awakenings need to be added here
         */
        auto maxLevel = evaluateNumericalFunction(maxLevelNumericalFunctionId, limitBreakCount);
        newLevel = std::min(*newLevel, maxLevel);
    }
    if(newLevel.has_value() && *newLevel != currentLevel) {
        m_log.debug("weapon %s has leveled up: %d -> %d", userWeaponUuid.c_str(), currentLevel, *newLevel);

        auto updateLevel = db().prepare("UPDATE i_user_weapon SET level = ? WHERE user_id = ? AND user_weapon_uuid = ?");
        updateLevel->bind(1, *newLevel);
        updateLevel->bind(2, m_userId);
        updateLevel->bind(3, userWeaponUuid);
        updateLevel->exec();

        auto updateNote = db().prepare("UPDATE i_user_weapon_note SET max_level = MAX(max_level, ?) WHERE user_id = ? AND weapon_id = ?");
        updateNote->bind(1, *newLevel);
        updateNote->bind(2, m_userId);
        updateNote->bind(3, weaponId);
        updateNote->exec();


        /*
         * TODO: level unlocks (if that needs to be done on the server - it probably needs to be
         */
        updateWeaponUnlockedStory(weaponId);
    }
}

void UserContext::serializeTable(const std::string &tableEntityName, LLServices::JSONWriter &json) {
    DatabaseContext::serializeTable(tableEntityName, json, m_userId);
}

void UserContext::updateDeckName(int32_t deckType, int32_t userDeckNumber, const std::string_view &deckName) {

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
    updateName->bind(1, m_userId);
    updateName->bind(2, deckType);
    updateName->bind(3, userDeckNumber);
    updateName->bind(4, deckName);
    updateName->exec();
}

void UserContext::refreshDeckPower(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::DeckPower &deckPower) {

    auto updateDeck = db().prepare(R"SQL(
        UPDATE i_user_deck SET
            power = ?
        WHERE
            user_id = ? AND
            deck_type = ? AND
            user_deck_number = ?
    )SQL");

    updateDeck->bind(1, deckPower.power());
    updateDeck->bind(2, m_userId);
    updateDeck->bind(3, deckType);
    updateDeck->bind(4, userDeckNumber);
    updateDeck->exec();

    if(deckPower.has_deck_character_power_01()) {
        updateDeckCharacterPower(deckPower.deck_character_power_01());
    }

    if(deckPower.has_deck_character_power_02()) {
        updateDeckCharacterPower(deckPower.deck_character_power_02());
    }

    if(deckPower.has_deck_character_power_03()) {
        updateDeckCharacterPower(deckPower.deck_character_power_03());
    }

    auto updateDeckNote = db().prepare(R"SQL(
        INSERT INTO i_user_deck_type_note (
            user_id,
            deck_type,
            max_deck_power
        ) VALUES (
            ?,
            ?,
            ?
        )
        ON CONFLICT (user_id, deck_type) DO UPDATE SET max_deck_power = MAX(max_deck_power, excluded.max_deck_power)
    )SQL");
    updateDeckNote->bind(1, m_userId);
    updateDeckNote->bind(2, deckType);
    updateDeckNote->bind(3, deckPower.power());
    updateDeckNote->exec();
}

void UserContext::updateDeckCharacterPower(const apb::api::deck::DeckCharacterPower &power) {
    auto updateCharacter = db().prepare(R"SQL(
        UPDATE i_user_deck_character SET
            power = ?
        WHERE
            user_id = ? AND
            user_deck_character_uuid = ?
    )SQL");
    updateCharacter->bind(1, power.power());
    updateCharacter->bind(2, m_userId);
    updateCharacter->bind(3, power.user_deck_character_uuid());
    updateCharacter->exec();
}

void UserContext::registerDokanConfirmed(int32_t dokanId) {

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_dokan (
            user_id,
            dokan_id,
            display_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, dokan_id) DO NOTHING
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, dokanId);
    query->exec();
}

bool UserContext::hasWeaponWithId(int32_t weaponId) {
    auto hasWeaponQuery = db().prepare("SELECT 1 FROM i_user_weapon WHERE user_id = ? AND weapon_id = ? LIMIT 1");
    hasWeaponQuery->bind(1, m_userId);
    hasWeaponQuery->bind(2, weaponId);
    if(hasWeaponQuery->step()) {
        return true;
    }

    return false;
}

bool UserContext::hasAnyCompanions() {
    auto checkAny = db().prepare("SELECT * FROM i_user_companion WHERE user_id = ?");
    checkAny->bind(1, m_userId);

    return checkAny->step();
}

void UserContext::registerNaviCutInPlayed(int32_t cutInId) {

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_navi_cut_in (
            user_id,
            navi_cut_in_id,
            play_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, navi_cut_in_id) DO NOTHING
    )SQL");

    query->bind(1, m_userId);
    query->bind(2, cutInId);
    query->exec();
}

void UserContext::registerOmikujiDraw(int32_t omikujiId) {

    auto draw = db().prepare(R"SQL(
        INSERT INTO i_user_omikuji (
            user_id,
            omikuji_id,
            latest_draw_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, omikuji_id) DO UPDATE SET latest_draw_datetime = excluded.latest_draw_datetime
    )SQL");
    draw->bind(1, m_userId);
    draw->bind(2, omikujiId);
    draw->exec();
}


void UserContext::gameStart() {

    for(auto userInitQuery: {
        /*
        * Set game_start_datetime if never initialized.
        */
        "UPDATE i_user SET game_start_datetime = current_net_timestamp() WHERE user_id = ? AND game_start_datetime < register_datetime",
        "INSERT OR IGNORE INTO i_user_tutorial_progress (user_id, tutorial_type, progress_phase, choice_id) VALUES (?, 1, 1, 0)",
        "INSERT OR IGNORE INTO i_user_setting (user_id) VALUES (?)",
        "INSERT OR IGNORE INTO i_user_main_quest_main_flow_status (user_id) VALUES (?)",
    }) {
        auto query = db().prepare(userInitQuery);
        query->bind(1, m_userId);
        query->exec();
    }

    /*
     * TODO: IUserShopReplaceableLineup is updated here
     * TODO: IUserShopReplaceable is updated here
     */
}

void UserContext::beforeGamePlay() {
    /*
     * Reset stamina if it was set in the future.
     */
    auto query = db().prepare(
        "UPDATE i_user_status SET stamina_milli_value = 50000 WHERE user_id = ? AND stamina_update_datetime >= current_net_timestamp()"
    );
    query->bind(1, m_userId);
    query->exec();
}

void UserContext::setUserName(const std::string_view &userName) {
    auto statement = db().prepare("UPDATE i_user_profile SET name = ? WHERE user_id = ?");
    statement->bind(1, userName);
    statement->bind(2, m_userId);

    statement->exec();
}

void UserContext::setUserMessage(const std::string_view &userMessage) {
    auto statement = db().prepare("UPDATE i_user_profile SET message = ? WHERE user_id = ?");
    statement->bind(1, userMessage);
    statement->bind(2, m_userId);

    statement->exec();
}

void UserContext::setUserFavoriteCostume(int32_t costumeId) {

    auto statement = db().prepare("UPDATE i_user_profile SET favorite_costume_id = ? WHERE user_id = ?");
    statement->bind(1, costumeId);
    statement->bind(2, m_userId);

    statement->exec();
}

void UserContext::setUserTutorialProgress(int32_t tutorialType, int32_t progressPhase, int32_t choiceId) {
    auto statement = db().prepare(R"SQL(
        INSERT INTO i_user_tutorial_progress (
            user_id,
            tutorial_type,
            progress_phase,
            choice_id
        ) VALUES (
            ?,
            ?,
            ?,
            ?
        )
        ON CONFLICT DO UPDATE SET
            progress_phase = excluded.progress_phase,
            choice_id =
                CASE
                    WHEN excluded.choice_id != 0 THEN excluded.choice_id
                    ELSE choice_id
                END
    )SQL");

    statement->bind(1, m_userId);
    statement->bind(2, tutorialType);
    statement->bind(3, progressPhase);
    statement->bind(4, choiceId);
    statement->exec();

    /*
     * It appears that in some cases we should be giving items depending on
     * the tutorial choices, but we don't seem to have the data needed.
     */
}


void UserContext::updateMainFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId) {

    leavePortalCage();

    auto updateMainFlow = db().prepare(R"SQL(
        UPDATE i_user_main_quest_main_flow_status SET
            current_quest_scene_id = ?,
            head_quest_scene_id = ?
        WHERE user_id = ?
    )SQL");

    updateMainFlow->bind(1, currentSceneId);
    updateMainFlow->bind(2, headSceneId);
    updateMainFlow->bind(3, m_userId);
    updateMainFlow->exec();

    setMainQuestFlowStatus(QuestFlowType::MAIN_FLOW);
}

void UserContext::updateReplayFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId) {
    /*
     * TODO: does this need anything special for the 'B side' of sun/moon?
     */

    leavePortalCage();

    auto updateReplayFlow = db().prepare(R"SQL(
        INSERT INTO i_user_main_quest_replay_flow_status (
            user_id,
            current_quest_scene_id,
            current_head_quest_scene_id
        ) VALUES (
            ?, ?, ?
        ) ON CONFLICT (user_id) DO UPDATE SET
            current_quest_scene_id = excluded.current_quest_scene_id,
            current_head_quest_scene_id = excluded.current_head_quest_scene_id
    )SQL");

    updateReplayFlow->bind(1, m_userId);
    updateReplayFlow->bind(2, currentSceneId);
    updateReplayFlow->bind(3, headSceneId);
    updateReplayFlow->exec();

    setMainQuestFlowStatus(QuestFlowType::REPLAY_FLOW);
}


void UserContext::recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber) {

    auto updateStartAttributes = db().prepare(R"SQL(
        INSERT INTO internal_user_quest_last_start_attributes (
            user_id,
            quest_id,
            user_deck_number
        ) VALUES (
            ?,
            ?,
            ?
        )
        ON CONFLICT (user_id, quest_id) DO UPDATE SET
            user_deck_number = excluded.user_deck_number
    )SQL");
    updateStartAttributes->bind(1, m_userId);
    updateStartAttributes->bind(2, questId);
    updateStartAttributes->bind(3, userDeckNumber);
    updateStartAttributes->exec();
}

void UserContext::startExtraQuest(int32_t questId) {

    auto firstScene = getFirstQuestScene(questId);

    setExtraQuestProgressStatus(questId, firstScene, firstScene);

    commonStartQuest(questId);
}

void UserContext::startEventQuest(int32_t eventQuestChapterId, int32_t questId, const std::optional<bool> &isBattleOnly) {

    auto firstScene = getFirstQuestScene(questId);

    setEventQuestProgressStatus(eventQuestChapterId, questId, firstScene, firstScene);

    commonStartQuest(questId, isBattleOnly);
}

void UserContext::commonStartQuest(int32_t questId, const std::optional<bool> &isBattleOnly) {

    auto startQuest = db().prepare(R"SQL(
        INSERT INTO i_user_quest (
            user_id,
            quest_id,
            quest_state_type,
            is_battle_only,
            latest_start_datetime
        ) VALUES (
            ?,
            ?,
            ?,
            COALESCE(?, 0),
            current_net_timestamp()
        )
        ON CONFLICT DO UPDATE SET
            quest_state_type = excluded.quest_state_type,
            is_battle_only = COALESCE(excluded.is_battle_only, is_battle_only),
            latest_start_datetime = excluded.latest_start_datetime
    )SQL");

    startQuest->bind(1, m_userId);
    startQuest->bind(2, questId);
    startQuest->bind(3, static_cast<int32_t>(QuestStateType::InProgress));
    if(isBattleOnly.has_value()) {
        startQuest->bind(4, *isBattleOnly ? 1 : 0);
    } else {
        startQuest->bindNull(4);
    }
    startQuest->exec();
}

void UserContext::startMainQuest(
    int32_t questId, bool isMainFlow, bool isReplayFlow, const std::optional<bool> &isBattleOnly) {

    leavePortalCage();

    auto route = getMainQuestRouteId(questId);

    /*
     * TODO: we currently select the first scene in a quest, and this is probably incorrect
     */
    /*
     * TODO: what's the distinction between 'current' scene and 'head' scene? Is it the 'highest reached?'
     */

    auto firstScene = getFirstQuestScene(questId);

    /*
     * TODO: questFlowType (0 - no active, 1 probably main, replay - ??)
     */
    QuestFlowType questFlowType = QuestFlowType::MAIN_FLOW;
    setMainQuestFlowStatus(questFlowType);

    setMainQuestProgressStatus(firstScene, firstScene, questFlowType);

    commonStartQuest(questId, isBattleOnly);

    if(isMainFlow) {

        auto updateMainFlow = db().prepare(R"SQL(
            INSERT INTO i_user_main_quest_main_flow_status (
                user_id,
                current_main_quest_route_id,
                current_quest_scene_id,
                head_quest_scene_id,
                is_reached_last_quest_scene
            ) VALUES (
                ?,
                ?,
                ?,
                ?,
                0
            )
            ON CONFLICT DO UPDATE SET
                current_main_quest_route_id = excluded.current_main_quest_route_id,
                current_quest_scene_id = excluded.current_quest_scene_id,
                head_quest_scene_id = excluded.head_quest_scene_id,
                is_reached_last_quest_scene = excluded.is_reached_last_quest_scene
        )SQL");

        updateMainFlow->bind(1, m_userId);
        updateMainFlow->bind(2, route);
        updateMainFlow->bind(3, firstScene);
        updateMainFlow->bind(4, firstScene);
        updateMainFlow->exec();
    }

    if(isReplayFlow) {
        auto updateReplayFlow = db().prepare(R"SQL(
            INSERT INTO i_user_main_quest_replay_flow_status (
                user_id,
                current_quest_scene_id,
                head_quest_scene_id
            ) VALUES (
                ?,
                ?,
                ?
            )
            ON CONFLICT DO UPDATE SET
                current_quest_scene_id = excluded.current_quest_scene_id,
                head_quest_scene_id = excluded.head_quest_scene_id
        )SQL");

        updateReplayFlow->bind(1, m_userId);
        updateReplayFlow->bind(2, firstScene);
        updateReplayFlow->bind(3, firstScene);
        updateReplayFlow->exec();
    }

    // TODO: should update i_user_quest_auto_orbit
}

void UserContext::getOrResetAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber) {
    auto getAttributesAtStart = db().prepare(R"SQL(
        SELECT user_deck_number FROM internal_user_quest_last_start_attributes
        WHERE
            user_id = ? AND
            quest_id = ?
    )SQL");
    getAttributesAtStart->bind(1, m_userId);
    getAttributesAtStart->bind(2, questId);

    if(getAttributesAtStart->step()) {
        userDeckNumber = getAttributesAtStart->columnInt(0);
    } else {
        m_log.error("getAttributesAtStartOfQuest (%d): the quest is not running\n", questId);

        userDeckNumber = 1;

        recordQuestStartAttributes(questId, userDeckNumber);
    }

    getAttributesAtStart->reset();
}

void UserContext::getAndClearAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber) {
    auto getAttributesAtStart = db().prepare(R"SQL(
        DELETE FROM
            internal_user_quest_last_start_attributes
        WHERE
            user_id = ? AND
            quest_id = ?
        RETURNING
            user_deck_number
    )SQL");
    getAttributesAtStart->bind(1, m_userId);
    getAttributesAtStart->bind(2, questId);

    userDeckNumber = 0;

    if(getAttributesAtStart->step()) {
        userDeckNumber = getAttributesAtStart->columnInt(0);
    }
}

void UserContext::retireQuest(int32_t questId) {
    auto updateQuest = db().prepare(R"SQL(
        UPDATE i_user_quest SET
            quest_state_type = CASE clear_count WHEN 0 THEN ? ELSE ? END
        WHERE user_id = ? AND quest_id = ? AND quest_state_type = ?
    )SQL");

    updateQuest->bind(1, static_cast<int32_t>(QuestStateType::NotStarted));
    updateQuest->bind(2, static_cast<int32_t>(QuestStateType::Cleared));
    updateQuest->bind(3, m_userId);
    updateQuest->bind(4, questId);
    updateQuest->bind(5, static_cast<int32_t>(QuestStateType::InProgress));
    updateQuest->exec();
}

void UserContext::finishQuest(
    int32_t questId,
    int32_t userDeckNumber,
    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *firstClearRewards,
    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *dropRewards) {

    auto getQuestRewardInfo = db().prepare(R"SQL(
        SELECT
            quest_first_clear_reward_group_id,
            quest_pickup_reward_group_id,
            gold,
            user_exp,
            character_exp,
            costume_exp,
            quest_deck_restriction_group_id
        FROM
            m_quest
        WHERE quest_id = ?
    )SQL");

    getQuestRewardInfo->bind(1, questId);
    if(!getQuestRewardInfo->step())
        throw std::runtime_error("no such quest");

    auto questFirstClearRewardGroupId = getQuestRewardInfo->columnInt(0);
    auto questPickupRewardGroupId = getQuestRewardInfo->columnInt(1);
    auto gold = getQuestRewardInfo->columnInt(2);
    auto userExperience = getQuestRewardInfo->columnInt(3);
    auto characterExperience = getQuestRewardInfo->columnInt(4);
    auto costumeExperience = getQuestRewardInfo->columnInt(5);
    auto restrictionGroup = getQuestRewardInfo->columnInt(6);

    int32_t deckType;
    if(restrictionGroup == 0)
        deckType = static_cast<int32_t>(DeckType::QUEST);
    else
        deckType = static_cast<int32_t>(DeckType::RESTRICTED_QUEST);

    auto updateQuest = db().prepare(R"SQL(
        UPDATE i_user_quest SET
            quest_state_type = ?,
            clear_count = clear_count + 1,
            last_clear_datetime = current_net_timestamp()
        WHERE user_id = ? AND quest_id = ? AND quest_state_type = ?
        RETURNING clear_count
    )SQL");

    updateQuest->bind(1, static_cast<int32_t>(QuestStateType::Cleared));
    updateQuest->bind(2, m_userId);
    updateQuest->bind(3, questId);
    updateQuest->bind(4, static_cast<int32_t>(QuestStateType::InProgress));
    while(updateQuest->step()) {
        auto clearCount = updateQuest->columnInt(0);
        if(clearCount == 1 && questFirstClearRewardGroupId != 0) {
            issueFirstClearRewardGroup(questFirstClearRewardGroupId, firstClearRewards, dropRewards);
        }
    }


    if(gold != 0) {
        givePossession(static_cast<int32_t>(PossessionType::CONSUMABLE_ITEM),
                       consumableItemIdForGold(),
                       gold
        );
    }

    if(userExperience != 0) {
        giveUserExperience(userExperience);
    }

    if(userDeckNumber != 0) {
        giveUserDeckExperience(deckType,
                               userDeckNumber,
                               characterExperience,
                               costumeExperience);

    }

    /*
     * TODO: This is simplification: add all battle drop.
     */
    auto getAllBattleDrop = db().prepare(R"SQL(
        SELECT
            possession_type, possession_id, count
        FROM
            m_quest_pickup_reward_group,
            m_battle_drop_reward ON m_battle_drop_reward.battle_drop_reward_id = m_quest_pickup_reward_group.battle_drop_reward_id

        WHERE quest_pickup_reward_group_id = ?
        ORDER BY sort_order
    )SQL");
    getAllBattleDrop->bind(1, questPickupRewardGroupId);
    while(getAllBattleDrop->step()) {
        auto type = getAllBattleDrop->columnInt(0);
        auto id = getAllBattleDrop->columnInt(1);
        auto count = getAllBattleDrop->columnInt(2);

        givePossession(type, id, count, dropRewards);
    }

}



void UserContext::updateMainQuestProgress() {
    /*
     * Get the selected route, if any.
     *
     * TODO: XXX: THIS IS INCORRECT
     */

    int64_t route = 0;

    auto getCurrentRoute = db().prepare("SELECT current_main_quest_route_id FROM i_user_main_quest_main_flow_status WHERE user_id = ?");
    getCurrentRoute->bind(1, m_userId);
    if(getCurrentRoute->step())
        route = getCurrentRoute->columnInt64(0);

    m_log.debug("current route: %ld", route);

    if(route != 0) {
        /*
         * Get all quests of the route, in order.
         * TODO: correct difficulty type???
         */

        auto allQuestsInOrder = db().prepare(R"SQL(
            SELECT
                m_main_quest_sequence.quest_id
            FROM
                m_main_quest_route,
                m_main_quest_chapter ON m_main_quest_chapter.main_quest_route_id = m_main_quest_route.main_quest_route_id,
                m_main_quest_sequence_group ON m_main_quest_sequence_group.main_quest_sequence_group_id = m_main_quest_chapter.main_quest_sequence_group_id,
                m_main_quest_sequence ON m_main_quest_sequence.main_quest_sequence_id = m_main_quest_sequence_group.main_quest_sequence_id
            WHERE
                m_main_quest_route.main_quest_route_id = ? AND
                start_datetime < current_net_timestamp() AND
                difficulty_type = 1
            ORDER BY
                m_main_quest_route.sort_order,
                m_main_quest_chapter.sort_order,
                m_main_quest_sequence.sort_order
        )SQL");
        allQuestsInOrder->bind(1, route);

        int64_t foundIncompleteQuest = 0;

        auto checkIfThisQuestComplete = db().prepare("SELECT quest_state_type FROM i_user_quest WHERE user_id = ? AND quest_id = ?");
        checkIfThisQuestComplete->bind(1, m_userId);

        while(allQuestsInOrder->step()) {
            auto questId = allQuestsInOrder->columnInt64(0);

            checkIfThisQuestComplete->bind(2, questId);
            int32_t stateType = static_cast<int32_t>(QuestStateType::NotStarted);

            if(checkIfThisQuestComplete->step()) {
                stateType = checkIfThisQuestComplete->columnInt(0);
            }
            checkIfThisQuestComplete->reset();

            m_log.debug("route quest %ld has state_type %d", questId, stateType);

            if(stateType < static_cast<int32_t>(QuestStateType::Cleared)) {
                /*
                 * The quest is not complete.
                 */
                foundIncompleteQuest = questId;
                break;
            }
        }

        allQuestsInOrder->reset();

        if(foundIncompleteQuest != 0) {
            m_log.debug("setting the main quest flow to start from %ld", foundIncompleteQuest);

            int64_t firstScene = 0 ;
            auto getFirstScene = db().prepare("SELECT quest_scene_id FROM m_quest_scene WHERE quest_id = ? ORDER BY sort_order LIMIT 1");
            getFirstScene->bind(1, foundIncompleteQuest);
            if(getFirstScene->step())
                firstScene = getFirstScene->columnInt64(0);

            m_log.debug("first scene: %ld", firstScene);

            if(firstScene != 0) {

                /*
                 * TODO: 'last quest scene' is probably 'end of route!'
                 */

                auto updateNextScene = db().prepare("UPDATE i_user_main_quest_main_flow_status SET current_quest_scene_id = ?, head_quest_scene_id = ? WHERE user_id = ?");
                updateNextScene->bind(1, firstScene);
                updateNextScene->bind(2, firstScene);
                updateNextScene->bind(3, m_userId);
                updateNextScene->exec();
            }
        }
    }

}

void UserContext::updateMainQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId) {

    auto update = db().prepare("UPDATE i_user_main_quest_progress_status SET current_quest_scene_id = ?, head_quest_scene_id = ? WHERE user_id = ?");

    update->bind(1, currentQuestSceneId);
    update->bind(2, headQuestSceneId);
    update->bind(3, m_userId);
    update->exec();

    updateUserUnlocks();
}

void UserContext::setMainQuestProgressStatus(int32_t currentQuestSceneId, int32_t headQuestSceneId, QuestFlowType currentQuestFlowType) {
    auto updateMainQuestProgressStatus = db().prepare(R"SQL(
        INSERT INTO i_user_main_quest_progress_status (
            user_id,
            current_quest_scene_id,
            head_quest_scene_id,
            current_quest_flow_type
        ) VALUES (
            ?,
            ?,
            ?,
            ?
        )
        ON CONFLICT DO UPDATE SET
            current_quest_scene_id = excluded.current_quest_scene_id,
            head_quest_scene_id = excluded.head_quest_scene_id,
            current_quest_flow_type = excluded.current_quest_flow_type
    )SQL");
    updateMainQuestProgressStatus->bind(1, m_userId);
    updateMainQuestProgressStatus->bind(2, currentQuestSceneId);
    updateMainQuestProgressStatus->bind(3, headQuestSceneId);
    updateMainQuestProgressStatus->bind(4, static_cast<int32_t>(currentQuestFlowType));
    updateMainQuestProgressStatus->exec();
}

void UserContext::setMainQuestFlowStatus(QuestFlowType flowType) {

    auto updateMainQuestFlowStatus = db().prepare(R"SQL(
        INSERT INTO i_user_main_quest_flow_status (
            user_id,
            current_quest_flow_type
        ) VALUES (
            ?,
            ?
        )
        ON CONFLICT DO UPDATE SET
            current_quest_flow_type = excluded.current_quest_flow_type
    )SQL");

    updateMainQuestFlowStatus->bind(1, m_userId);
    updateMainQuestFlowStatus->bind(2, static_cast<int32_t>(flowType));
    updateMainQuestFlowStatus->exec();
}


void UserContext::issueFirstClearRewardGroup(int64_t firstClearGroupId,
                                              google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards,
                                              google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestDropRewards) {

    m_log.debug("issuing first clear reward group %ld", firstClearGroupId);

    auto getRewardGroup = db().prepare(R"SQL(
        SELECT
            quest_first_clear_reward_type,
            possession_type,
            possession_id,
            count,
            is_pickup
        FROM m_quest_first_clear_reward_group
        WHERE quest_first_clear_reward_group_id = ?
        ORDER BY sort_order
    )SQL");
    getRewardGroup->bind(1, firstClearGroupId);

    while(getRewardGroup->step()) {
        auto rewardType = getRewardGroup->columnInt(0);
        auto possessionType = getRewardGroup->columnInt(1);
        auto possessionId = getRewardGroup->columnInt(2);
        auto count = getRewardGroup->columnInt(3);
        auto isPickup = getRewardGroup->columnInt(4);

        /*
         * TODO: reward type handling
         */

        m_log.debug("awarding first clear reward: reward type %d, possession type %d, possession id %d, count %d, is pickup? %d",
               rewardType, possessionType, possessionId, count, isPickup);

        auto list = addToQuestRewards;
        if(isPickup) {
            list = addToQuestDropRewards;
        }

        givePossession(possessionType, possessionId, count, list);
    }
}


void UserContext::updateWeaponUnlockedStory(int32_t weaponId) {
    auto getUserWeaponStory = db().prepare("SELECT released_max_story_index FROM i_user_weapon_story WHERE user_id = ? AND weapon_id = ?");
    getUserWeaponStory->bind(1, m_userId);

    auto getWeaponStoryConditionReleaseGroupId = db().prepare("SELECT weapon_story_release_condition_group_id FROM m_weapon WHERE weapon_id = ?");

    auto getConditionsToReleaseNextStories = db().prepare(R"SQL(
        SELECT
            story_index,
            weapon_story_release_condition_type,
            condition_value,
            weapon_story_release_condition_operation_group_id
        FROM m_weapon_story_release_condition_group
        WHERE
            weapon_story_release_condition_group_id = ? AND
            story_index > ?
        ORDER BY story_index
    )SQL");

    auto getReleaseConditionOperationGroupOperationType = db().prepare(R"SQL(
        SELECT
            condition_operation_type
        FROM
            m_weapon_story_release_condition_operation_group
        WHERE
            weapon_story_release_condition_operation_group_id = ?
    )SQL");

    auto getReleaseConditionOperation = db().prepare(R"SQL(
        SELECT
            weapon_story_release_condition_type,
            condition_value
        FROM
            m_weapon_story_release_condition_operation
        WHERE
            weapon_story_release_condition_operation_group_id = ?
        ORDER BY sort_order
    )SQL");

    auto update = db().prepare(R"SQL(
        INSERT INTO i_user_weapon_story (
            user_id,
            weapon_id,
            released_max_story_index
        ) VALUES (
            ?,
            ?,
            ?
        )
        ON CONFLICT (user_id, weapon_id) DO UPDATE SET
            released_max_story_index = excluded.released_max_story_index
    )SQL");

    getWeaponStoryConditionReleaseGroupId->bind(1, weaponId);
    int32_t weaponStoryReleaseConditionGroupId = 0;

    if(getWeaponStoryConditionReleaseGroupId->step()) {
        weaponStoryReleaseConditionGroupId = getWeaponStoryConditionReleaseGroupId->columnInt(0);
    }
    getWeaponStoryConditionReleaseGroupId->reset();

    if(weaponStoryReleaseConditionGroupId == 0)
        return;

    getUserWeaponStory->bind(2, weaponId);
    int32_t releasedMaxStoryIndex = 0;
    if(getUserWeaponStory->step()) {
        releasedMaxStoryIndex = getUserWeaponStory->columnInt(0);
    }
    getUserWeaponStory->reset();

    m_log.debug("weapon %d, story release condition group ID: %d currently released story index: %d",
            weaponId,
            weaponStoryReleaseConditionGroupId, releasedMaxStoryIndex);

    getConditionsToReleaseNextStories->bind(1, weaponStoryReleaseConditionGroupId);
    getConditionsToReleaseNextStories->bind(2, releasedMaxStoryIndex);

    std::optional<int> newlyReleasedStoryIndex;

    update->bind(1, m_userId);

    while(getConditionsToReleaseNextStories->step()) {
        auto nextStoryIndex = getConditionsToReleaseNextStories->columnInt(0);
        auto weaponStoryReleaseConditionType = getConditionsToReleaseNextStories->columnInt(1);
        auto conditionValue = getConditionsToReleaseNextStories->columnInt(2);
        auto releaseConditionOperationGroupId = getConditionsToReleaseNextStories->columnInt(3);

        m_log.debug("next story index to release: %d, condition type: %d, condition value: %d, release condition operation group ID: %d",
                nextStoryIndex, weaponStoryReleaseConditionType, conditionValue, releaseConditionOperationGroupId);

        bool released;

        if(releaseConditionOperationGroupId != 0) {
            // Multiple conditions.
            ConditionOperationType conditionOperationType = ConditionOperationType::UNKNOWN;

            getReleaseConditionOperationGroupOperationType->bind(1, releaseConditionOperationGroupId);
            if(getReleaseConditionOperationGroupOperationType->step()) {
                conditionOperationType = static_cast<ConditionOperationType>(getReleaseConditionOperationGroupOperationType->columnInt(0));
            }

            getReleaseConditionOperationGroupOperationType->reset();

            getReleaseConditionOperation->bind(1, releaseConditionOperationGroupId);

            if(conditionOperationType == ConditionOperationType::AND) {
                released = true;

                while(getReleaseConditionOperation->step()) {
                    auto type = static_cast<WeaponStoryReleaseConditionType>(getReleaseConditionOperation->columnInt(0));
                    auto value = getReleaseConditionOperation->columnInt(1);

                    auto thisReleased = isWeaponStoryReleaseConditionSatisfied(weaponId, type, value);
                    released = released && thisReleased;
                }

            } else if(conditionOperationType == ConditionOperationType::OR) {

                released = false;

                while(getReleaseConditionOperation->step()) {
                    auto type = static_cast<WeaponStoryReleaseConditionType>(getReleaseConditionOperation->columnInt(0));
                    auto value = getReleaseConditionOperation->columnInt(1);

                    auto thisReleased = isWeaponStoryReleaseConditionSatisfied(weaponId, type, value);
                    released = released || thisReleased;
                }

            } else {
                m_log.error("unsupported weapon story condition operation type: %d", conditionOperationType);
                break;
            }

            getReleaseConditionOperation->reset();

        } else {
            // Single condition.

            released = isWeaponStoryReleaseConditionSatisfied(
                weaponId,
                static_cast<WeaponStoryReleaseConditionType>(weaponStoryReleaseConditionType), conditionValue);
        }

        if(!released)
            break;

        m_log.debug("weapon story index %d should be released", nextStoryIndex);
        newlyReleasedStoryIndex.emplace(nextStoryIndex);

    }

    getConditionsToReleaseNextStories->reset();

    if(newlyReleasedStoryIndex.has_value()) {
        m_log.debug("updating released story index for %d to %d", weaponId, *newlyReleasedStoryIndex);

        update->bind(2, weaponId);
        update->bind(3, *newlyReleasedStoryIndex);
        update->exec();
        update->reset();
    }
}

void UserContext::updateUserUnlocks() {
    auto getUserWeapons = db().prepare("SELECT DISTINCT weapon_id FROM i_user_weapon WHERE user_id = ?");
    getUserWeapons->bind(1, m_userId);

    while(getUserWeapons->step()) {
        auto weaponId = getUserWeapons->columnInt(0);
        updateWeaponUnlockedStory(weaponId);
    }
    getUserWeapons->reset();

    auto getUserCharacters = db().prepare("SELECT character_id FROM i_user_character WHERE user_id = ?");
    getUserCharacters->bind(1, m_userId);

    while(getUserCharacters->step()) {
        auto characterId = getUserCharacters->columnInt(0);
        reevaluateCharacterCostumeLevelBonuses(characterId);
    }
    getUserCharacters->reset();
}

bool UserContext::isQuestCleared(int32_t questId) {
    auto query = db().prepare("SELECT quest_state_type FROM i_user_quest WHERE quest_id = ?");
    query->bind(1, questId);
    if(query->step()) {
        return query->columnInt(0) >= static_cast<int32_t>(QuestStateType::Cleared);
    }

    return false;
}

std::optional<int32_t> UserContext::getWeaponLevel(int32_t weaponId) {
    auto query = db().prepare("SELECT MAX(level) FROM i_user_weapon WHERE user_id = ? AND weapon_id = ?");
    query->bind(1, m_userId);
    query->bind(2, weaponId);
    if(query->step())
        return query->columnInt(0);

    return std::nullopt;
}

bool UserContext::isWeaponStoryReleaseConditionSatisfied(
    int32_t weaponId,
    WeaponStoryReleaseConditionType type,
    int32_t value
) {
    switch(type) {
        case WeaponStoryReleaseConditionType::ACQUISITION:
            return hasWeaponWithId(weaponId);

        case WeaponStoryReleaseConditionType::REACH_SPECIFIED_LEVEL:
            return getWeaponLevel(weaponId).value_or(0) >= value;

        // TODO: REACH_INITIAL_MAX_LEVEL
        // TODO: REACH_ONCE_EVOLVED_MAX_LEVEL
        // TODO: REACH_SPECIFIED_EVOLUTION_COUNT

        case WeaponStoryReleaseConditionType::QUEST_CLEAR:
            return isQuestCleared(value);

        // MAIN_FLOW_SCENE_PROGRESS is problematic to implement, but appears unused anyway

        default:
            m_log.error("isWeaponStoryReleaseConditionSatisfied: unsupported condition type %d", type);
            return false;
    }
}


void UserContext::registerCostumeLevelBonusConfirmed(int32_t costumeId, int32_t newlyConfirmedLevel) {
    auto query = db().prepare(R"SQL(
        UPDATE i_user_costume_level_bonus_release_status SET
            confirmed_bonus_level = MAX(confirmed_bonus_level, ?)
        WHERE
            user_id = ? AND
            costume_id = ?
    )SQL");
    query->bind(1, newlyConfirmedLevel);
    query->bind(2, m_userId);
    query->bind(3, costumeId);
    query->exec();
}


void UserContext::queryCostumeEnhancementCost(const std::string &costumeUUID, int32_t itemCount,
                                              int32_t &costumeEnhancementCost) {
    auto query = db().prepare("SELECT costume_id FROM i_user_costume WHERE user_id = ? AND user_costume_uuid = ?");
    query->bind(1, m_userId);
    query->bind(2, costumeUUID);
    if(query->step())
        return DatabaseContext::queryCostumeEnhancementCost(query->columnInt(0), itemCount, costumeEnhancementCost);

    throw std::runtime_error("the user doesn't have such costume");

}

void UserContext::queryWeaponEnhancementCost(const std::string &weaponUUID, int32_t itemCount,
                                             int32_t &weaponEnhancementCost) {
    auto query = db().prepare("SELECT weapon_id FROM i_user_weapon WHERE user_id = ? AND user_weapon_uuid = ?");
    query->bind(1, m_userId);
    query->bind(2, weaponUUID);
    if(query->step())
        return DatabaseContext::queryWeaponEnhancementCost(query->columnInt(0), itemCount, weaponEnhancementCost);

    throw std::runtime_error("the user doesn't have such weapon");

}

int32_t UserContext::consumeEnhancementMaterialAndCalculateTotalEffectValue(int32_t materialId, int32_t materialCount, MaterialType requiredMaterialType) {


    if(materialCount <= 0)
        throw std::logic_error("material count is not valid");

    auto checkMaterialSuitabilityAndAvailability = db().prepare(R"SQL(
        SELECT
            effect_value
        FROM
            i_user_material,
            m_material ON m_material.material_id = i_user_material.material_id
        WHERE
            user_id = ? AND
            i_user_material.material_id = ? AND
            count >= ? AND
            material_type = ?
    )SQL");
    checkMaterialSuitabilityAndAvailability->bind(1, m_userId);
    checkMaterialSuitabilityAndAvailability->bind(2, materialId);
    checkMaterialSuitabilityAndAvailability->bind(3, materialCount);
    checkMaterialSuitabilityAndAvailability->bind(4, static_cast<int32_t>(requiredMaterialType));

    if(!checkMaterialSuitabilityAndAvailability->step())
        throw std::runtime_error("the user doesn't have enough of the material, or the material is not appropriate");


    auto value = checkMaterialSuitabilityAndAvailability->columnInt(0);

    checkMaterialSuitabilityAndAvailability->reset();

    auto consumeMaterial = db().prepare(R"SQL(
        UPDATE
            i_user_material
        SET
            count = count - ?
        WHERE
            user_id = ? AND
            material_id = ?
        RETURNING
            count
    )SQL");
    consumeMaterial->bind(1, materialCount);
    consumeMaterial->bind(2, m_userId);
    consumeMaterial->bind(3, materialId);

    if(!consumeMaterial->step())
        throw std::runtime_error("could not consume the material");

    return value * materialCount;
}

void UserContext::consumeMaterial(int32_t materialId, int32_t count) {
    if(count < 0)
        throw std::logic_error("the count to consume is not valid");

    if(count == 0)
        return;

    auto checkEnoughCount = db().prepare(R"SQL(
        SELECT
            count
        FROM i_user_material
        WHERE
            user_id = ? AND
            material_id = ? AND
            count >= ?
    )SQL");
    checkEnoughCount->bind(1, m_userId);
    checkEnoughCount->bind(2, materialId);
    checkEnoughCount->bind(3, count);
    if(!checkEnoughCount->step())
        throw std::runtime_error("not enough of the consumable item available");

    checkEnoughCount->reset();

    auto consume = db().prepare(R"SQL(
        UPDATE i_user_material SET
            count = count - ?
        WHERE
            user_id = ? AND
            material_id = ?
    )SQL");
    consume->bind(1, count);
    consume->bind(2, m_userId);
    consume->bind(3, materialId);
    consume->exec();
}

void UserContext::consumeConsumableItem(int32_t consumableItemId, int32_t count) {
    if(count < 0)
        throw std::logic_error("the count to consume is not valid");

    if(count == 0)
        return;

    auto checkEnoughCount = db().prepare(R"SQL(
        SELECT
            count
        FROM i_user_consumable_item
        WHERE
            user_id = ? AND
            consumable_item_id = ? AND
            count >= ?
    )SQL");
    checkEnoughCount->bind(1, m_userId);
    checkEnoughCount->bind(2, consumableItemId);
    checkEnoughCount->bind(3, count);
    if(!checkEnoughCount->step())
        throw std::runtime_error("not enough of the consumable item available");

    checkEnoughCount->reset();

    auto consume = db().prepare(R"SQL(
        UPDATE i_user_consumable_item SET
            count = count - ?
        WHERE
            user_id = ? AND
            consumable_item_id = ?
    )SQL");
    consume->bind(1, count);
    consume->bind(2, m_userId);
    consume->bind(3, consumableItemId);
    consume->exec();
}

void UserContext::setWeaponProtected(const std::string_view &uuid, bool isProtected) {
    auto query = db().prepare("UPDATE i_user_weapon SET is_protected = ? WHERE user_id = ? AND user_weapon_uuid = ?");
    query->bind(1, isProtected);
    query->bind(2, m_userId);
    query->bind(3, uuid);
    query->exec();
}

void UserContext::enhanceCostumeActiveSkill(const std::string_view &uuid) {
    auto queryStats = db().prepare(R"SQL(
        SELECT
            -- from i_user_costume_active_skill, current level
            i_user_costume_active_skill.level AS skill_level,
            -- from m_costume_rarity
            active_skill_max_level_numerical_function_id,
            active_skill_enhancement_cost_numerical_function_id,

            (
                SELECT
                    costume_active_skill_enhancement_material_id
                FROM
                    m_costume_active_skill_group
                WHERE
                    costume_active_skill_group_id = m_costume.costume_active_skill_group_id AND
                    costume_limit_break_count_lower_limit <= limit_break_count
                ORDER BY costume_limit_break_count_lower_limit DESC
                LIMIT 1
            ) AS costume_active_skill_enhancement_material_id
        FROM
            i_user_costume,
            i_user_costume_active_skill USING (user_id, user_costume_uuid),
            m_costume USING (costume_id),
            m_costume_rarity USING (rarity_type)
        WHERE
            user_id = ? AND -- TODO: parameter
            user_costume_uuid = ?
    )SQL");

    queryStats->bind(1, m_userId);
    queryStats->bind(2, uuid);
    if(!queryStats->step())
        throw std::runtime_error("no such costume");

    auto currentSkillLevel = queryStats->columnInt(0);
    auto maxLevelFunction = queryStats->columnInt(1);
    auto enhancementCostFunction = queryStats->columnInt(2);
    auto enhancementMaterial = queryStats->columnInt(3);
    queryStats->reset();


    /*
     * Check to be constant 1 in Subsystem.Serval.CostumeServal::calcActiveSkillMaxLevel
     */
    auto maxLevel = evaluateNumericalFunction(maxLevelFunction, 1);
    if(currentSkillLevel >= maxLevel)
        throw std::runtime_error("already at the max level");

    auto cost = evaluateNumericalFunction(enhancementCostFunction, currentSkillLevel + 1); /* checked to be current *plus one* */

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto listMaterials = db().prepare(R"SQL(
        SELECT
            material_id,
            count
        FROM
            m_costume_active_skill_enhancement_material
        WHERE
            costume_active_skill_enhancement_material_id = ? AND
            skill_level = ?
        ORDER BY sort_order
    )SQL");
    listMaterials->bind(1, enhancementMaterial);
    listMaterials->bind(2, currentSkillLevel); /* checked to be *current* */

    while(listMaterials->step()) {
        consumeMaterial(listMaterials->columnInt(0), listMaterials->columnInt(1));
    }
    listMaterials->reset();

    auto updateLevel = db().prepare(R"SQL(
        UPDATE i_user_costume_active_skill SET
            level = level + 1
        WHERE
            user_id = ? AND
            user_costume_uuid = ?
    )SQL");
    updateLevel->bind(1, m_userId);
    updateLevel->bind(2, uuid);
    updateLevel->exec();
}

void UserContext::enhanceWeaponSkill(const std::string_view &uuid, int32_t skillId) {
    auto queryStats = db().prepare(R"SQL(
        SELECT
            i_user_weapon_skill.level AS skill_level,
            max_skill_level_numerical_function_id,
            skill_enhancement_cost_numerical_function_id,
            weapon_skill_enhancement_material_id,
            limit_break_count,
            slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_rarity USING (rarity_type),
            m_weapon_skill_group USING (weapon_skill_group_id),
            i_user_weapon_skill USING (user_id, user_weapon_uuid, slot_number)
        WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            skill_id = ?
    )SQL");
    queryStats->bind(1, m_userId);
    queryStats->bind(2, uuid);
    queryStats->bind(3, skillId);
    if(!queryStats->step())
        throw std::runtime_error("no such weapon or skill");

    auto currentSkillLevel = queryStats->columnInt(0);
    auto maxLevelFunction = queryStats->columnInt(1);
    auto enhancementCostFunction = queryStats->columnInt(2);
    auto enhancementMaterial = queryStats->columnInt(3);
    auto limitBreakCount = queryStats->columnInt(4);
    auto slotNumber = queryStats->columnInt(5);
    queryStats->reset();

    auto maxLevel = evaluateNumericalFunction(maxLevelFunction, limitBreakCount);
    if(currentSkillLevel >= maxLevelFunction)
        throw std::runtime_error("already at the max level");

    auto cost = evaluateNumericalFunction(enhancementCostFunction, currentSkillLevel + 1); /* checked to be current *plus one* */

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto listMaterials = db().prepare(R"SQL(
        SELECT
            material_id,
            count
        FROM
            m_weapon_skill_enhancement_material
        WHERE
            weapon_skill_enhancement_material_id = ? AND
            skill_level = ?
        ORDER BY sort_order
    )SQL");
    listMaterials->bind(1, enhancementMaterial);
    listMaterials->bind(2, currentSkillLevel); /* checked to be *current* */

    while(listMaterials->step()) {
        consumeMaterial(listMaterials->columnInt(0), listMaterials->columnInt(1));
    }
    listMaterials->reset();

    auto updateLevel = db().prepare(R"SQL(
        UPDATE i_user_weapon_skill SET
            level = level + 1
        WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            slot_number = ?
    )SQL");
    updateLevel->bind(1, m_userId);
    updateLevel->bind(2, uuid);
    updateLevel->bind(3, slotNumber);
    updateLevel->exec();
}

void UserContext::enhanceWeaponAbility(const std::string_view &uuid, int32_t abilityId) {
    auto queryStats = db().prepare(R"SQL(
        SELECT
            i_user_weapon_ability.level AS ability_level,
            max_ability_level_numerical_function_id,
            ability_enhancement_cost_numerical_function_id,
            weapon_ability_enhancement_material_id,
            limit_break_count,
            slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_rarity USING (rarity_type),
            m_weapon_ability_group USING (weapon_ability_group_id),
            i_user_weapon_ability USING (user_id, user_weapon_uuid, slot_number)
        WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            ability_id = ?
    )SQL");
    queryStats->bind(1, m_userId);
    queryStats->bind(2, uuid);
    queryStats->bind(3, abilityId);
    if(!queryStats->step())
        throw std::runtime_error("no such weapon or ability");

    auto currentAbilityLevel = queryStats->columnInt(0);
    auto maxLevelFunction = queryStats->columnInt(1);
    auto enhancementCostFunction = queryStats->columnInt(2);
    auto enhancementMaterial = queryStats->columnInt(3);
    auto limitBreakCount = queryStats->columnInt(4);
    auto slotNumber = queryStats->columnInt(5);
    queryStats->reset();

    auto maxLevel = evaluateNumericalFunction(maxLevelFunction, limitBreakCount);
    if(currentAbilityLevel >= maxLevelFunction)
        throw std::runtime_error("already at the max level");

    auto cost = evaluateNumericalFunction(enhancementCostFunction, currentAbilityLevel + 1); /* checked to be current *plus one* */

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto listMaterials = db().prepare(R"SQL(
        SELECT
            material_id,
            count
        FROM
            m_weapon_ability_enhancement_material
        WHERE
            weapon_ability_enhancement_material_id = ? AND
            ability_level = ?
        ORDER BY sort_order
    )SQL");
    listMaterials->bind(1, enhancementMaterial);
    listMaterials->bind(2, currentAbilityLevel); /* checked to be *current* */

    while(listMaterials->step()) {
        consumeMaterial(listMaterials->columnInt(0), listMaterials->columnInt(1));
    }
    listMaterials->reset();

    auto updateLevel = db().prepare(R"SQL(
        UPDATE i_user_weapon_ability SET
            level = level + 1
        WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            slot_number = ?
    )SQL");
    updateLevel->bind(1, m_userId);
    updateLevel->bind(2, uuid);
    updateLevel->bind(3, slotNumber);
    updateLevel->exec();
}

void UserContext::updatePortalCageSceneProgress(int32_t sceneId) {
    auto updatePortalCage = db().prepare(R"SQL(
        INSERT INTO i_user_portal_cage_status (
            user_id,
            is_current_progress
        ) VALUES (
            ?,
            1
        ) ON CONFLICT DO UPDATE SET is_current_progress = excluded.is_current_progress
    )SQL");
    updatePortalCage->bind(1, m_userId);
    updatePortalCage->exec();
}

void UserContext::setMainQuestRoute(int32_t routeId) {

    auto getRouteSeason = db().prepare("SELECT main_quest_season_id FROM m_main_quest_route WHERE main_quest_route_id = ?");
    getRouteSeason->bind(1, routeId);
    if(!getRouteSeason->step())
        throw std::runtime_error("no such route");

    auto seasonId = getRouteSeason->columnInt(0);
    getRouteSeason->reset();

    auto updateUserRoute = db().prepare(R"SQL(
        INSERT INTO i_user_main_quest_season_route (
            user_id,
            main_quest_season_id,
            main_quest_route_id
        ) VALUES (
            ?,
            ?,
            ?
        ) ON CONFLICT (user_id, main_quest_season_id) DO UPDATE SET
            main_quest_route_id = excluded.main_quest_route_id
    )SQL");
    updateUserRoute->bind(1, m_userId);
    updateUserRoute->bind(2, seasonId);
    updateUserRoute->bind(3, routeId);
    updateUserRoute->exec();

    // TODO: do we need to update 'i_user_main_quest_main_flow_status' here?
}

void UserContext::leavePortalCage() {
    auto query = db().prepare(R"SQL(
        UPDATE i_user_portal_cage_status SET
            is_current_progress = 0
        WHERE
            user_id = ? AND
            is_current_progress = 1
    )SQL");
    query->bind(1, m_userId);
    query->exec();
}

void UserContext::recordCageOrnamentAccess(int32_t cageOrnamentId) {
    /*
     * TODO: It's currently unclear what's the difference between 'record access' and
     * 'receive reward' for the cage ornaments. So we currently set acquisition_datetime
     * to zero for RecordAccess calls so they can be located and fixed up later, if necessary.
     *
     * But it's definite that a i_user_cage_ornament_reward record *must* be
     * made for a RecordAccess call, though.
     */

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_cage_ornament_reward (
            user_id,
            cage_ornament_id,
            acquisition_datetime
        ) VALUES (
            ?,
            ?,
            0
        )
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, cageOrnamentId);
    query->exec();
}

void UserContext::costumeLimitBreak(const std::string &costumeUUID,
                                    const google::protobuf::Map<int32_t, int32_t> &materialsToUse) {

/*
 * TODO: we probably could validate the material set, but it doesn't really matter.
 */

    auto getCostumeStatus = db().prepare(
        "SELECT costume_id, limit_break_count FROM i_user_costume WHERE user_id = ? AND user_costume_uuid = ?"
    );
    getCostumeStatus->bind(1, m_userId);
    getCostumeStatus->bind(2, costumeUUID);
    if(!getCostumeStatus->step())
        throw std::runtime_error("no such costume");

    auto costumeId = getCostumeStatus->columnInt(0);
    auto currentLimitBreakCount = getCostumeStatus->columnInt(1);
    getCostumeStatus->reset();

    if(currentLimitBreakCount >= getIntConfig("COSTUME_LIMIT_BREAK_AVAILABLE_COUNT"))
        throw std::runtime_error("already at the maximum count of limit breaks");

    auto getLimitBreakCostInformation = db().prepare(R"SQL(
        SELECT
            limit_break_cost_numerical_function_id
        FROM
            m_costume,
            m_costume_rarity USING (rarity_type)
        WHERE
            costume_id = ?
    )SQL");
    getLimitBreakCostInformation->bind(1, costumeId);
    if(!getLimitBreakCostInformation->step())
        throw std::runtime_error("no such costume");

    auto limitBreakCostNumericalFunctionId = getLimitBreakCostInformation->columnInt(0);
    getLimitBreakCostInformation->reset();

    int32_t materialCount = 0;

    for(const auto &pair: materialsToUse) {
        materialCount += pair.second;
        consumeMaterial(pair.first, pair.second);
    }


    auto cost = evaluateNumericalFunction(limitBreakCostNumericalFunctionId, materialCount);

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto updateLimitBreakCount = db().prepare(R"SQL(
        UPDATE i_user_costume SET
            limit_break_count = limit_break_count + 1
        WHERE
            user_id = ? AND
            user_costume_uuid = ?
    )SQL");
    updateLimitBreakCount->bind(1, m_userId);
    updateLimitBreakCount->bind(2, costumeUUID);
    updateLimitBreakCount->exec();

    /*
     * Force the level to reevaluate.
     */
    giveUserCostumeExperience(costumeUUID, 0, 0);
}

void UserContext::weaponLimitBreak(const std::string &weaponUUID,
                                    const google::protobuf::Map<int32_t, int32_t> &materialsToUse) {

/*
 * TODO: we probably could validate the material set, but it doesn't really matter.
 */

    auto getCostumeStatus = db().prepare(
        "SELECT weapon_id, limit_break_count FROM i_user_weapon WHERE user_id = ? AND user_weapon_uuid = ?"
    );
    getCostumeStatus->bind(1, m_userId);
    getCostumeStatus->bind(2, weaponUUID);
    if(!getCostumeStatus->step())
        throw std::runtime_error("no such weapon");

    auto weaponId = getCostumeStatus->columnInt(0);
    auto currentLimitBreakCount = getCostumeStatus->columnInt(1);
    getCostumeStatus->reset();

    if(currentLimitBreakCount >= getIntConfig("WEAPON_LIMIT_BREAK_AVAILABLE_COUNT"))
        throw std::runtime_error("already at the maximum count of limit breaks");

    auto getLimitBreakCostInformation = db().prepare(R"SQL(
        SELECT
            limit_break_cost_by_material_numerical_function_id
        FROM
            m_weapon,
            m_weapon_rarity USING (rarity_type)
        WHERE
            weapon_id = ?
    )SQL");
    getLimitBreakCostInformation->bind(1, weaponId);
    if(!getLimitBreakCostInformation->step())
        throw std::runtime_error("no such weapon");

    auto limitBreakCostNumericalFunctionId = getLimitBreakCostInformation->columnInt(0);
    getLimitBreakCostInformation->reset();

    int32_t materialCount = 0;

    for(const auto &pair: materialsToUse) {
        materialCount += pair.second;
        consumeMaterial(pair.first, pair.second);
    }


    auto cost = evaluateNumericalFunction(limitBreakCostNumericalFunctionId, materialCount);

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto updateLimitBreakCount = db().prepare(R"SQL(
        UPDATE i_user_weapon SET
            limit_break_count = limit_break_count + 1
        WHERE
            user_id = ? AND
            user_weapon_uuid = ?
    )SQL");
    updateLimitBreakCount->bind(1, m_userId);
    updateLimitBreakCount->bind(2, weaponUUID);
    updateLimitBreakCount->exec();

    /*
     * Force the level to reevaluate.
     */
    giveUserWeaponExperience(weaponUUID, 0, 0);
}

void UserContext::enhanceCompanion(const std::string &companionUUID) {
    auto queryCompanionParameters = db().prepare(R"SQL(
        SELECT
            level,
            enhancement_cost_numerical_function_id,
            companion_category_type
        FROM
            i_user_companion,
            m_companion USING (companion_id),
            m_companion_category USING (companion_category_type)
        WHERE
            user_id = ? AND
            user_companion_uuid = ?
    )SQL");
    queryCompanionParameters->bind(1, m_userId);
    queryCompanionParameters->bind(2, companionUUID);
    if(!queryCompanionParameters->step())
        throw std::runtime_error("no such companion");

    auto currentLevel = queryCompanionParameters->columnInt(0);
    auto enhancementCostFunction = queryCompanionParameters->columnInt(1);
    auto categoryType = queryCompanionParameters->columnInt(2);

    queryCompanionParameters->reset();

    auto enhancementCost = evaluateNumericalFunction(enhancementCostFunction, currentLevel);
    consumeConsumableItem(consumableItemIdForGold(), enhancementCost);

    auto materialQuery = db().prepare(R"SQL(
        SELECT
            material_id,
            count
        FROM m_companion_enhancement_material
        WHERE
            companion_category_type = ? AND
            level = ?
        ORDER BY sort_order
    )SQL");
    materialQuery->bind(1, categoryType);
    materialQuery->bind(2, currentLevel);


    bool gotAnyMaterials = false;
    while(materialQuery->step()) {
        gotAnyMaterials = true;

        auto material = materialQuery->columnInt(0);
        auto count = materialQuery->columnInt(1);

        consumeMaterial(material, count);
    }

    if(!gotAnyMaterials)
        throw std::runtime_error("the companion cannot be enhanced past the current level");

    auto updateLevel = db().prepare(R"SQL(
        UPDATE i_user_companion SET level = level + 1 WHERE user_id = ? AND user_companion_uuid = ?
    )SQL");
    updateLevel->bind(1, m_userId);
    updateLevel->bind(2, companionUUID);
    updateLevel->exec();
}

void UserContext::setQuestSceneChoice(
    int32_t questSceneId,
    int32_t choiceNumber,
    int32_t questFlowType) {

    /*
     * TODO: This possibly needs to give items?
     */

    auto getChoiceData = db().prepare(R"SQL(
        SELECT
            m_quest_scene_choice.quest_scene_choice_effect_id,
            quest_scene_choice_grouping_id
        FROM
            m_quest_scene_choice,
            m_quest_scene_choice_effect USING(quest_scene_choice_effect_id)
        WHERE
            main_flow_quest_scene_id = ? AND
            quest_flow_type = ? AND
            choice_number = ?
    )SQL");

    getChoiceData->bind(1, questSceneId);
    getChoiceData->bind(2, questFlowType);
    getChoiceData->bind(3, choiceNumber);
    if(!getChoiceData->step())
        throw std::runtime_error("no such quest choice");

    auto effectId = getChoiceData->columnInt(0);
    auto groupingId = getChoiceData->columnInt(1);
    getChoiceData->reset();

    auto recordChoice = db().prepare(R"SQL(
        INSERT INTO i_user_quest_scene_choice (
            user_id,
            quest_scene_choice_grouping_id,
            quest_scene_choice_effect_id
        ) VALUES (
            ?,
            ?,
            ?
        )
        ON CONFLICT (user_id, quest_scene_choice_grouping_id) DO UPDATE SET
            quest_scene_choice_effect_id = excluded.quest_scene_choice_effect_id
    )SQL");
    recordChoice->bind(1, m_userId);
    recordChoice->bind(2, groupingId);
    recordChoice->bind(3, effectId);
    recordChoice->exec();

    auto recordHistory = db().prepare(R"SQL(
        INSERT INTO i_user_quest_scene_choice_history (
            user_id,
            quest_scene_choice_effect_id,
            choice_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, quest_scene_choice_effect_id) DO UPDATE SET
            choice_datetime = excluded.choice_datetime
    )SQL");

    recordHistory->bind(1, m_userId);
    recordHistory->bind(2, effectId);
    recordHistory->exec();
}

void UserContext::setExtraQuestProgressStatus(int32_t currentQuestId, int32_t currentQuestSceneId, int32_t headQuestSceneId) {
    auto setStatus = db().prepare(R"SQL(
        INSERT INTO i_user_extra_quest_progress_status (
            user_id,
            current_quest_id,
            current_quest_scene_id,
            head_quest_scene_id
        ) VALUES (
            ?, ?, ?, ?
        )
        ON CONFLICT (user_id) DO UPDATE SET
            current_quest_id = excluded.current_quest_id,
            current_quest_scene_id = excluded.current_quest_scene_id,
            head_quest_scene_id = excluded.head_quest_scene_id
    )SQL");
    setStatus->bind(1, m_userId);
    setStatus->bind(2, currentQuestId);
    setStatus->bind(3, currentQuestSceneId);
    setStatus->bind(4, headQuestSceneId);
    setStatus->exec();
}

void UserContext::setEventQuestProgressStatus(int32_t eventQuestChapterId, int32_t currentQuestId, int32_t currentQuestSceneId,
                                              int32_t headQuestSceneId) {
    auto setStatus = db().prepare(R"SQL(
        INSERT INTO i_user_event_quest_progress_status (
            user_id,
            current_event_quest_chapter_id,
            current_quest_id,
            current_quest_scene_id,
            head_quest_scene_id
        ) VALUES (
            ?, ?, ?, ?, ?
        )
        ON CONFLICT (user_id) DO UPDATE SET
            current_quest_id = excluded.current_quest_id,
            current_event_quest_chapter_id = excluded.current_event_quest_chapter_id,
            current_quest_scene_id = excluded.current_quest_scene_id,
            head_quest_scene_id = excluded.head_quest_scene_id
    )SQL");
    setStatus->bind(1, m_userId);
    setStatus->bind(2, eventQuestChapterId);
    setStatus->bind(3, currentQuestId);
    setStatus->bind(4, currentQuestSceneId);
    setStatus->bind(5, headQuestSceneId);
    setStatus->exec();
}

void UserContext::updateExtraQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId) {
    auto updateProgress = db().prepare(R"SQL(
        UPDATE i_user_extra_quest_progress_status SET
            current_quest_scene_id = ?,
            head_quest_scene_id = ?
        WHERE
            user_id = ? AND
            current_quest_id > 0
        RETURNING current_quest_id
    )SQL");

    updateProgress->bind(1, currentQuestSceneId);
    updateProgress->bind(2, headQuestSceneId);
    updateProgress->bind(3, m_userId);

    if(!updateProgress->step())
        throw std::runtime_error("no extra quest is currently active");

    updateProgress->exec();
}

void UserContext::updateEventQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId) {
    auto updateProgress = db().prepare(R"SQL(
        UPDATE i_user_event_quest_progress_status SET
            current_quest_scene_id = ?,
            head_quest_scene_id = ?
        WHERE
            user_id = ? AND
            current_quest_id > 0
        RETURNING current_quest_id
    )SQL");

    updateProgress->bind(1, currentQuestSceneId);
    updateProgress->bind(2, headQuestSceneId);
    updateProgress->bind(3, m_userId);

    if(!updateProgress->step())
        throw std::runtime_error("no event quest is currently active");

    updateProgress->exec();
}

void UserContext::reevaluateCharacterCostumeLevelBonuses(int32_t character) {
    m_log.debug("reevaluating character %d costume level bonuses", character);

    auto getCostumes = db().prepare(
        "SELECT costume_level_bonus_id, level FROM i_user_costume, m_costume USING (costume_id) WHERE user_id = ? AND character_id = ?"
    );
    getCostumes->bind(1, m_userId);
    getCostumes->bind(2, character);

    auto getBonuses = db().prepare(
        "SELECT costume_level_bonus_type, effect_value FROM m_costume_level_bonus WHERE costume_level_bonus_id = ? AND level <= ?"
    );

    /*
     * While support for multiplicative bonuses exists in the database
     * schema, it's never actually used: there's no costumes with
     * multiplicative bonuses configured, the client never fetches the
     * corresponding aggregated record, and the live server always fills it
     * with zeroes. We don't emit it at all.
     */
    struct AggregatedBonuses {
        int32_t hp = 0;
        int32_t attack = 0;
        int32_t vitality = 0;
        int32_t agility = 0;
        int32_t criticalRatio = 0;
        int32_t criticalAttack = 0;
    } bonuses;

    while(getCostumes->step()) {
        auto costumeLevelBonusId = getCostumes->columnInt(0);
        auto costumeLevel = getCostumes->columnInt(1);

        getBonuses->bind(1, costumeLevelBonusId);
        getBonuses->bind(2, costumeLevel);

        m_log.debug("  including a costume: level %d, bonus ID %d", costumeLevel, costumeLevelBonusId);

        while(getBonuses->step()) {
            auto bonusType = static_cast<CostumeLevelBonusType>(getBonuses->columnInt(0));
            auto bonusValue = static_cast<int32_t>(getBonuses->columnInt(1));

            switch(bonusType) {
            case CostumeLevelBonusType::AGILITY_ADD:
                bonuses.agility += bonusValue;
                break;

            case CostumeLevelBonusType::ATTACK_ADD:
                bonuses.attack += bonusValue;
                break;

            case CostumeLevelBonusType::CRITICAL_ATTACK_ADD:
                bonuses.criticalAttack += bonusValue;
                break;

            case CostumeLevelBonusType::CRITICAL_RATIO_ADD:
                bonuses.criticalRatio += bonusValue;
                break;

            case CostumeLevelBonusType::HP_ADD:
                bonuses.hp += bonusValue;
                break;

            case CostumeLevelBonusType::VITALITY_ADD:
                bonuses.vitality += bonusValue;
                break;

            default:
                throw std::runtime_error("unsupported bonus type " + std::to_string(static_cast<int32_t>(bonusType)));
            }
        }

        getBonuses->reset();
    }

    auto storeBonuses = db().prepare(R"SQL(
        INSERT INTO i_user_character_costume_level_bonus (
            user_id,
            character_id,
            status_calculation_type,
            hp,
            attack,
            vitality,
            agility,
            critical_ratio,
            critical_attack
        ) VALUES (
            ?, ?, ?, ?, ?, ?, ?, ?, ?
        ) ON CONFLICT (user_id, character_id, status_calculation_type) DO UPDATE SET
            hp = excluded.hp,
            attack = excluded.attack,
            vitality = excluded.vitality,
            agility = excluded.agility,
            critical_ratio = excluded.critical_ratio,
            critical_attack = excluded.critical_attack
    )SQL");
    storeBonuses->bind(1, m_userId);
    storeBonuses->bind(2, character);
    storeBonuses->bind(3, static_cast<int32_t>(StatusCalculationType::ADD));
    storeBonuses->bind(4, bonuses.hp);
    storeBonuses->bind(5, bonuses.attack);
    storeBonuses->bind(6, bonuses.vitality);
    storeBonuses->bind(7, bonuses.agility);
    storeBonuses->bind(8, bonuses.criticalRatio);
    storeBonuses->bind(9, bonuses.criticalAttack);
    storeBonuses->exec();
}

void UserContext::rebirthCharacter(int32_t characterId) {
    int32_t rebirthCount = 0;

    auto getCurrentRebirthCount = db().prepare("SELECT rebirth_count FROM i_user_character_rebirth WHERE user_id = ? AND character_id = ?");
    getCurrentRebirthCount->bind(1, m_userId);
    getCurrentRebirthCount->bind(2, characterId);
    if(getCurrentRebirthCount->step()) {
        rebirthCount = getCurrentRebirthCount->columnInt(0);
    }

    getCurrentRebirthCount->reset();

    if(rebirthCount >= getIntConfig("CHARACTER_REBIRTH_AVAILABLE_COUNT"))
        throw std::runtime_error("the character is already at the max number of rebirths");

    auto getRebirthMaterialGroup = db().prepare(R"SQL(
        SELECT
            character_rebirth_material_group_id
        FROM
            m_character_rebirth,
            m_character_rebirth_step_group USING (character_rebirth_step_group_id)
        WHERE
            character_id = ? AND
            before_rebirth_count = ?
    )SQL");
    getRebirthMaterialGroup->bind(1, characterId);
    getRebirthMaterialGroup->bind(2, rebirthCount);
    if(!getRebirthMaterialGroup->step())
        throw std::runtime_error("no such rebirth step");

    auto rebirthMaterialGroupId = getRebirthMaterialGroup->columnInt(0);
    getRebirthMaterialGroup->reset();

    auto getMaterials = db().prepare(
        "SELECT material_id, count FROM m_character_rebirth_material_group WHERE character_rebirth_material_group_id = ? ORDER BY sort_order"
    );
    getMaterials->bind(1, rebirthMaterialGroupId);

    while(getMaterials->step()) {
        auto materialId = getMaterials->columnInt(0);
        auto count = getMaterials->columnInt(1);

        consumeMaterial(materialId, count);
    }

    consumeConsumableItem(consumableItemIdForGold(), getIntConfig("CHARACTER_REBIRTH_CONSUME_GOLD"));

    auto updateRebirths = db().prepare(R"SQL(
        INSERT INTO i_user_character_rebirth (
            user_id,
            character_id,
            rebirth_count
        ) VALUES (
            ?, ?, ?
        )
        ON CONFLICT (user_id, character_id) DO UPDATE SET
            rebirth_count = excluded.rebirth_count
    )SQL");

    updateRebirths->bind(1, m_userId);
    updateRebirths->bind(2, characterId);
    updateRebirths->bind(3, rebirthCount + 1);
    updateRebirths->exec();

    /*
     * Rebirthing a character increases the level limit of all the costumes
     * of that character, so the costumes of the character just reborn need
     * their levels reevaluated.
     */

    auto getCostumeUUIDs = db().prepare(
        "SELECT user_costume_uuid FROM i_user_costume, m_costume USING (costume_id) WHERE user_id = ? AND character_id = ?"
    );

    getCostumeUUIDs->bind(1, m_userId);
    getCostumeUUIDs->bind(2, characterId);
    while(getCostumeUUIDs->step()) {
        auto costumeUUID = getCostumeUUIDs->columnText(0);

        giveUserCostumeExperience(costumeUUID, 0, 0);
    }
}

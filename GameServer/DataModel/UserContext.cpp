#include "service/GiftService.pb.h"
#include <DataModel/UserContext.h>
#include <DataModel/DatabaseEnums.h>
#include <DataModel/DatabaseJSONRepresentation.h>

#include <DataModel/Sqlite/Statement.h>
#include <DataModel/Sqlite/Transaction.h>

#include <service/DeckService.pb.h>
#include <service/QuestService.pb.h>
#include <service/CageOrnamentService.pb.h>
#include <service/GimmickService.pb.h>

#include <stdexcept>
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

            populateWeaponNote(weaponUuid);

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

            populateWeaponSkillsAndAbilities(weaponUuid);

            updateWeaponUnlockedStory(possessionId);
        }
        break;

        case PossessionType::COMPANION:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for COMPANION");

            giveUserCompanion(possessionId);
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

        case PossessionType::COMPANION_ENHANCED:
        {

            if(count != 1)
                throw std::runtime_error("Unexpected count value for COMPANION_ENHANCED");

            giveUserCompanionEnhanced(possessionId);

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
            query->reset();

            /*
             * If this is a stained glass item, it will affect character stats.
             * Query the affected characters and reevaluate.
             */

            auto queryAffectedCharacters = db().prepare(R"SQL(
                SELECT DISTINCT target_value
                FROM
                    m_important_item,
                    m_stained_glass ON stained_glass_id = external_reference_id,
                    m_stained_glass_status_up_target_group USING (stained_glass_status_up_target_group_id)
                WHERE
                    important_item_id = ? AND
                    important_item_type = 5 AND -- no enum for this, but assuming that's 'stained glass'
                    status_up_target_type = 1 -- CHARACTER
            )SQL");
            queryAffectedCharacters->bind(1, possessionId);
            while(queryAffectedCharacters->step()) {
                auto character = queryAffectedCharacters->columnInt(0);
                reevaluateCharacterCostumeLevelBonuses(character);
            }
            queryAffectedCharacters->reset();
        }
        break;


        case PossessionType::THOUGHT:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for THOUGHT");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_thought (
                    user_id,
                    user_thought_uuid,
                    thought_id,
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

        case PossessionType::PREMIUM_ITEM:
        {
            if(count != 1)
                throw std::runtime_error("Unexpected count value for PREMIUM_ITEM");

            auto query = db().prepare(R"SQL(
                INSERT INTO i_user_premium_item (
                    user_id,
                    premium_item_id,
                    acquisition_datetime
                ) VALUES (
                    ?,
                    ?,
                    current_net_timestamp()
                )
            )SQL");

            query->bind(1, m_userId);
            query->bind(2, possessionId);
            query->exec();
            break;
        }

        default:
            throw std::runtime_error("unsupported possession type " + std::to_string(possessionType));
    }

    buildDefaultDeckIfNoneExists();
}

void UserContext::giveUserCompanionEnhanced(int32_t companionEnhancedId) {
    auto query = db().prepare("SELECT companion_id, level FROM m_companion_enhanced WHERE companion_enhanced_id = ?");
    query->bind(1, companionEnhancedId);
    if(!query->step())
        throw std::runtime_error("no such enhanced companion");

    auto companionId = query->columnInt(0);
    auto level = query->columnInt(1);
    query->reset();

    giveUserCompanion(companionId, level);
}

void UserContext::giveUserCompanion(int32_t companionId, int32_t level) {

    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_companion (
            user_id,
            user_companion_uuid,
            companion_id,
            level,
            acquisition_datetime
        ) VALUES (
            ?,
            hex(randomblob(16)),
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, companion_id) DO NOTHING
        RETURNING user_companion_uuid
    )SQL");

    query->bind(1, m_userId);
    query->bind(2, companionId);
    query->bind(3, level);
    if(!query->step()) {
        m_log.warning("giveUserCompanion: user already has the companion %d",
                    companionId);

        auto duplicationItems = db().prepare(
            "SELECT possession_type, possession_id, count FROM m_companion_duplication_exchange_possession_group WHERE companion_id = ?"
        );
        duplicationItems->bind(1, companionId);
        while(duplicationItems->step()) {
            auto type = duplicationItems->columnInt(0);
            auto possessionId = duplicationItems->columnInt(1);
            auto count = duplicationItems->columnInt(2);
            givePossession(type, possessionId, count);
        }

        return;
    }
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
        ON CONFLICT (user_id, costume_id) DO NOTHING
        RETURNING user_costume_uuid
    )SQL");

    query->bind(1, m_userId);
    query->bind(2, costumeId);
    query->bind(3, limitBreakCount);
    query->bind(4, level);
    query->bind(5, *exp);
    query->bind(6, awakenCount);

    std::string costumeUUID;
    if(!query->step()) {
        m_log.warning("giveUserCostume: user already has the costume %d",
                    costumeId);

        auto duplicationItems = db().prepare(
            "SELECT possession_type, possession_id, count FROM m_costume_duplication_exchange_possession_group WHERE costume_id = ?"
        );
        duplicationItems->bind(1, costumeId);
        while(duplicationItems->step()) {
            auto type = duplicationItems->columnInt(0);
            auto possessionId = duplicationItems->columnInt(1);
            auto count = duplicationItems->columnInt(2);
            givePossession(type, possessionId, count);
        }

        return;
    }

    costumeUUID = query->columnText(0);

    query->reset();

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

    auto queryWeaponMaxLevel = db().prepare(R"SQL(
        SELECT
            COALESCE(
                m_weapon_specific_enhance.max_level_numerical_function_id,
                m_weapon_rarity.max_level_numerical_function_id)
        FROM
            m_weapon,
            m_weapon_rarity USING(rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
        WHERE
            weapon_id = ?
    )SQL");
    queryWeaponMaxLevel->bind(1, weaponId);
    if(!queryWeaponMaxLevel->step())
        throw std::runtime_error("the weapon setup was not found");

    int32_t maxLevelNumericalFunctionId = queryWeaponMaxLevel->columnInt(0);
    queryWeaponMaxLevel->reset();

    /*
     * Evolved weapons use the level table of their base evolution, i.e., the original rarity.
     */
    auto baseWeaponId = getUnevolvedWeaponId(weaponId);
    auto queryWeaponLevelTable = db().prepare(R"SQL(
        SELECT
            COALESCE(
                m_weapon_specific_enhance.required_exp_for_level_up_numerical_parameter_map_id,
                m_weapon_rarity.required_exp_for_level_up_numerical_parameter_map_id)
        FROM
            m_weapon,
            m_weapon_rarity USING(rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
        WHERE
            weapon_id = ?
    )SQL");
    queryWeaponLevelTable->bind(1, baseWeaponId);
    if(!queryWeaponLevelTable->step())
        throw std::runtime_error("the weapon setup was not found");

    int32_t weaponExpLevelMap = queryWeaponLevelTable->columnInt(0);
    queryWeaponLevelTable->reset();

    auto newLevel = evaluateNumericalParameterMap(weaponExpLevelMap, newExperience);
    if(newLevel.has_value()) {
        /*
         * TODO: awakenings need to be added here
         */
        auto maxLevel = evaluateNumericalFunction(maxLevelNumericalFunctionId, limitBreakCount);

        auto checkIfAwakened = db().prepare("SELECT 1 FROM i_user_weapon_awaken WHERE user_id = ? AND user_weapon_uuid = ?");
        checkIfAwakened->bind(1, m_userId);
        checkIfAwakened->bind(2, userWeaponUuid);
        if(checkIfAwakened->step()) {
            auto getLevelBoost = db().prepare("SELECT level_limit_up FROM m_weapon_awaken WHERE weapon_id = ?");
            getLevelBoost->bind(1, weaponId);
            if(!getLevelBoost->step())
                throw std::runtime_error("awakened weapon has no awakening setup");

            maxLevel += getLevelBoost->columnInt(0);
            getLevelBoost->reset();
        }
        checkIfAwakened->reset();

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

void UserContext::updateTripleDeckName(int32_t deckType, int32_t userDeckNumber, const std::string_view &deckName) {

    auto updateName = db().prepare(R"SQL(
        INSERT INTO i_user_triple_deck (
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

void UserContext::updateTripleDeck(int32_t deckType, int32_t userDeckNumber, int32_t deckNumber1, int32_t deckNumber2, int32_t deckNumber3) {
    auto updateDeck = db().prepare(R"SQL(
        INSERT INTO i_user_triple_deck (
            user_id,
            deck_type,
            user_deck_number,
            deck_number01,
            deck_number02,
            deck_number03
        ) VALUES (
            ?, ?, ?, ?, ?, ?
        )
        ON CONFLICT (user_id, deck_type, user_deck_number) DO UPDATE SET
            deck_number01 = excluded.deck_number01,
            deck_number02 = excluded.deck_number02,
            deck_number03 = excluded.deck_number03
    )SQL");
    updateDeck->bind(1, m_userId);
    updateDeck->bind(2, deckType);
    updateDeck->bind(3, userDeckNumber);
    updateDeck->bind(4, deckNumber1);
    updateDeck->bind(5, deckNumber2);
    updateDeck->bind(6, deckNumber3);
    updateDeck->exec();
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

    leaveSpecialStates();

    auto getCurrent = db().prepare("SELECT current_quest_scene_id FROM i_user_main_quest_main_flow_status WHERE user_id = ?");
    getCurrent->bind(1, m_userId);
    if(!getCurrent->step())
        throw std::runtime_error("no i_user_main_quest_main_flow_status record");

    auto currentlySetScene = getCurrent->columnInt(0);
    getCurrent->reset();

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

    if(currentlySetScene == 0) {


        /*
        * is_gift, is_debug aren't used.
        */
        auto getPossessions = db().prepare(
            "SELECT possession_type, possession_id, count FROM m_user_quest_scene_grant_possession WHERE quest_scene_id = ?"
        );
        getPossessions->bind(1, currentSceneId);
        while(getPossessions->step()) {
            auto type = getPossessions->columnInt(0);
            auto id = getPossessions->columnInt(1);
            auto count = getPossessions->columnInt(2);

            givePossession(type, id, count);
        }
    }
}

void UserContext::updateReplayFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId) {
    /*
     * TODO: does this need anything special for the 'B side' of sun/moon?
     */

    leaveSpecialStates();

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

    leaveSpecialStates();

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
    QuestFlowType questFlowType = QuestFlowType::UNKNOWN;

    if(isMainFlow) {
        questFlowType = QuestFlowType::MAIN_FLOW;
    } else if(isReplayFlow) {
        questFlowType = QuestFlowType::REPLAY_FLOW;
    }

    setMainQuestFlowStatus(questFlowType);

    setMainQuestProgressStatus(firstScene, firstScene, questFlowType);

    commonStartQuest(questId, isBattleOnly);

    if(isMainFlow) {

        auto route = getMainQuestRouteId(questId);

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
                current_head_quest_scene_id
            ) VALUES (
                ?,
                ?,
                ?
            )
            ON CONFLICT DO UPDATE SET
                current_quest_scene_id = excluded.current_quest_scene_id,
                current_head_quest_scene_id = excluded.current_head_quest_scene_id
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
    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *dropRewards,
    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *replayFlowFirstClearReward) {

    auto getQuestRewardInfo = db().prepare(R"SQL(
        SELECT
            quest_first_clear_reward_group_id,
            quest_pickup_reward_group_id,
            gold,
            user_exp,
            character_exp,
            costume_exp,
            quest_deck_restriction_group_id,
            quest_replay_flow_reward_group_id
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
    auto questReplayFlowRewardGroupId = getQuestRewardInfo->columnInt(7);

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
    int32_t clearCount = 0;
    while(updateQuest->step()) {
        clearCount = updateQuest->columnInt(0);
    }

    updateQuest->reset();

    if(replayFlowFirstClearReward) {
        /*
         * In the replay flow, we issue the replay flow rewards on the each
         * clear of *this specific replay flow quest*, that is, this one
         * quest_id.
         *
         * TODO: it's possible that the response reward list needs to be
         * different.
         */

        if(questReplayFlowRewardGroupId != 0) {
            issueReplayFlowRewardGroup(questReplayFlowRewardGroupId, replayFlowFirstClearReward);
        }

        /*
         * Now, for the purposes of determining whether the first clear
         * rewards should be given, the normal difficulty replay flow quest
         * is considered to be the 'same' as the main flow quest, even though
         * the quest ID is going to be different.
         *
         * So we need to add the clear count of the corresponding main flow
         * quest, if the quest we're completing now has normal difficulty.
         */

        auto getMainFlowClears = db().prepare(R"SQL(
            SELECT COALESCE(SUM(clear_count), 0)
            FROM
                m_quest_relation_main_flow,
                i_user_quest ON i_user_quest.quest_id = m_quest_relation_main_flow.main_flow_quest_id
            WHERE
                user_id = ? AND
                replay_flow_quest_id = ? AND
                difficulty_type = ?
        )SQL");
        getMainFlowClears->bind(1, m_userId);
        getMainFlowClears->bind(2, questId);
        getMainFlowClears->bind(3, static_cast<int32_t>(DifficultyType::NORMAL));
        if(getMainFlowClears->step()) {
            clearCount += getMainFlowClears->columnInt(0);
        }
    }


    if(clearCount == 1 && questFirstClearRewardGroupId != 0) {
        /*
         * TODO: it's possible that *for the replay flow* the response reward
         * list needs to be different.
         */
        issueFirstClearRewardGroup(questFirstClearRewardGroupId, firstClearRewards, dropRewards);
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

void UserContext::issueReplayFlowRewardGroup(int64_t replayFlowGroupId,
                                              google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards) {

    m_log.debug("issuing replay flow reward group %ld", replayFlowGroupId);

    auto getRewardGroup = db().prepare(R"SQL(
        SELECT
            possession_type,
            possession_id,
            count
        FROM m_quest_replay_flow_reward_group
        WHERE quest_replay_flow_reward_group_id = ?
        ORDER BY sort_order
    )SQL");
    getRewardGroup->bind(1, replayFlowGroupId);

    while(getRewardGroup->step()) {
        auto possessionType = getRewardGroup->columnInt(0);
        auto possessionId = getRewardGroup->columnInt(1);
        auto count = getRewardGroup->columnInt(2);

        givePossession(possessionType, possessionId, count, addToQuestRewards);
    }
}

void UserContext::updateWeaponUnlockedStory(int32_t weaponId) {
    weaponId = getUnevolvedWeaponId(weaponId);

    m_log.debug("updating weapon story unlocks for %d\n", weaponId);

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
    auto query = db().prepare(R"SQL(
        SELECT
            COALESCE(first_evolution.weapon_id, i_user_weapon.weapon_id) AS aggregate_weapon_id,
            MAX(level)
        FROM
            i_user_weapon LEFT JOIN
            m_weapon_evolution_group ON
                m_weapon_evolution_group.weapon_id = i_user_weapon.weapon_id
            LEFT JOIN
            m_weapon_evolution_group AS first_evolution ON
                first_evolution.weapon_evolution_group_id = m_weapon_evolution_group.weapon_evolution_group_id AND
                first_evolution.evolution_order = 1
        WHERE user_id = ? AND aggregate_weapon_id = ?
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, weaponId);

    while(query->step()) {
        auto effectiveWeaponId = query->columnInt(0);
        auto level = query->columnInt(1);

        return level;
    }

    return std::nullopt;
}

bool UserContext::isWeaponStoryReleaseConditionSatisfied(
    int32_t weaponId,
    WeaponStoryReleaseConditionType type,
    int32_t value
) {
    switch(type) {
        case WeaponStoryReleaseConditionType::ACQUISITION:
            return getWeaponLevel(weaponId).has_value();

        case WeaponStoryReleaseConditionType::REACH_SPECIFIED_LEVEL:
            return getWeaponLevel(weaponId).value_or(0) >= value;

        case WeaponStoryReleaseConditionType::REACH_INITIAL_MAX_LEVEL:
            return getWeaponLevel(weaponId).value_or(0) >= getWeaponMaxLevelForEvolutionOrder(weaponId, 1);

        case WeaponStoryReleaseConditionType::REACH_ONCE_EVOLVED_MAX_LEVEL:
            return getWeaponLevel(weaponId).value_or(0) >= getWeaponMaxLevelForEvolutionOrder(weaponId, 2);

        case WeaponStoryReleaseConditionType::REACH_SPECIFIED_EVOLUTION_COUNT:
            return getHighestEvolutionOrder(weaponId) >= value + 1;

        case WeaponStoryReleaseConditionType::QUEST_CLEAR:
            return isQuestCleared(value);

        // MAIN_FLOW_SCENE_PROGRESS is problematic to implement, but appears unused anyway

        default:
            m_log.error("isWeaponStoryReleaseConditionSatisfied: unsupported condition type %d, value %d", type, value);
            return false;
    }
}

int32_t UserContext::getHighestEvolutionOrder(int32_t weaponId) {
    auto query = db().prepare(R"SQL(
        SELECT
            MAX(other_weapon_evolution_group.evolution_order)
        FROM
            m_weapon_evolution_group AS requested_weapon_evolution_group,
            m_weapon_evolution_group AS other_weapon_evolution_group ON
                other_weapon_evolution_group.weapon_evolution_group_id = requested_weapon_evolution_group.weapon_evolution_group_id,
            i_user_weapon ON
                user_id = ? AND
                i_user_weapon.weapon_id = other_weapon_evolution_group.weapon_id
        WHERE
            requested_weapon_evolution_group.weapon_id = ?
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, weaponId);
    if(query->step())
        return query->columnInt(0);

    return 1;
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
            COALESCE(
                m_weapon_specific_enhance.max_skill_level_numerical_function_id,
                m_weapon_rarity.max_skill_level_numerical_function_id
            ),
            COALESCE(
                m_weapon_specific_enhance.skill_enhancement_cost_numerical_function_id,
                m_weapon_rarity.skill_enhancement_cost_numerical_function_id
            ),
            weapon_skill_enhancement_material_id,
            limit_break_count,
            slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_rarity USING (rarity_type),
            m_weapon_skill_group USING (weapon_skill_group_id),
            i_user_weapon_skill USING (user_id, user_weapon_uuid, slot_number) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
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
            COALESCE(
                m_weapon_specific_enhance.max_ability_level_numerical_function_id,
                m_weapon_rarity.max_ability_level_numerical_function_id
            ),
            COALESCE(
                m_weapon_specific_enhance.ability_enhancement_cost_numerical_function_id,
                m_weapon_rarity.ability_enhancement_cost_numerical_function_id
            ),
            weapon_ability_enhancement_material_id,
            limit_break_count,
            slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_rarity USING (rarity_type),
            m_weapon_ability_group USING (weapon_ability_group_id),
            i_user_weapon_ability USING (user_id, user_weapon_uuid, slot_number) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
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
    leaveSpecialStates();

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

void UserContext::leaveSpecialStates() {
    for(const char *text : {
        "UPDATE i_user_portal_cage_status SET is_current_progress = 0 WHERE user_id = ? AND is_current_progress = 1",
        "DELETE FROM i_user_side_story_quest_scene_progress_status WHERE user_id = ?",
    }) {

        auto query = db().prepare(text);
        query->bind(1, m_userId);
        query->exec();
    }
}

void UserContext::activateCageOrnament(int32_t cageOrnamentId,
                                       google::protobuf::RepeatedPtrField<apb::api::cageornament::CageOrnamentReward> *rewards) {
    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_cage_ornament_reward (
            user_id,
            cage_ornament_id,
            acquisition_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        ) ON CONFLICT (user_id, cage_ornament_id) DO NOTHING
        RETURNING 1
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, cageOrnamentId);

    if(query->step()) {
        /*
         * This is the first time this ornament was activated.
         * Query the rewards.
         */

        auto getRewards = db().prepare(R"SQL(
            SELECT
                possession_type,
                possession_id,
                count
            FROM
                m_cage_ornament,
                m_cage_ornament_reward USING (cage_ornament_reward_id)
            WHERE cage_ornament_reward_id = ?
        )SQL");
        getRewards->bind(1, cageOrnamentId);
        while(getRewards->step()) {
            auto possessionType = getRewards->columnInt(0);
            auto possessionId = getRewards->columnInt(1);
            auto count = getRewards->columnInt(2);

            givePossession(possessionType, possessionId, count);

            if(rewards) {
                auto reward = rewards->Add();
                reward->set_possession_type(possessionType);
                reward->set_possession_id(possessionId);
                reward->set_count(count);
            }
        }

        getRewards->reset();
    }

    query->reset();
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
            COALESCE(
                m_weapon_specific_enhance.limit_break_cost_by_material_numerical_function_id,
                m_weapon_rarity.limit_break_cost_by_material_numerical_function_id
            )
        FROM
            m_weapon,
            m_weapon_rarity USING (rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
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

template<typename T>
void UserContext::AggregatedBonuses::apply(T bonusType, int32_t bonusValue) {
    /*
     * While support for multiplicative bonuses exists in the database
     * schema, it's never actually used: there's no costumes or character
     * board panels with multiplicative bonuses configured, the client never
     * fetches the corresponding aggregated records, and the live server
     * always fills them with zeroes. We don't emit them at all.
     */


    switch(bonusType) {
    case T::AGILITY_ADD:
        agility += bonusValue;
        break;

    case T::ATTACK_ADD:
        attack += bonusValue;
        break;

    case T::CRITICAL_ATTACK_ADD:
        criticalAttack += bonusValue;
        break;

    case T::CRITICAL_RATIO_ADD:
        criticalRatio += bonusValue;
        break;

    case T::HP_ADD:
        hp += bonusValue;
        break;

    case T::VITALITY_ADD:
        vitality += bonusValue;
        break;

    default:
        throw std::runtime_error("unsupported bonus type " + std::to_string(static_cast<int32_t>(bonusType)));
    }
}

template<typename T>
void UserContext::AggregatedBonuses::applyKind(T bonusType, int32_t bonusValue) {

    switch(bonusType) {
    case T::AGILITY:
        agility += bonusValue;
        break;

    case T::ATTACK:
        attack += bonusValue;
        break;

    case T::CRITICAL_ATTACK:
        criticalAttack += bonusValue;
        break;

    case T::CRITICAL_RATIO:
        criticalRatio += bonusValue;
        break;

    case T::HP:
        hp += bonusValue;
        break;

    case T::VITALITY:
        vitality += bonusValue;
        break;

    default:
        throw std::runtime_error("unsupported bonus type " + std::to_string(static_cast<int32_t>(bonusType)));
    }
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

    AggregatedBonuses bonuses;

    while(getCostumes->step()) {
        auto costumeLevelBonusId = getCostumes->columnInt(0);
        auto costumeLevel = getCostumes->columnInt(1);

        getBonuses->bind(1, costumeLevelBonusId);
        getBonuses->bind(2, costumeLevel);

        m_log.debug("  including a costume: level %d, bonus ID %d", costumeLevel, costumeLevelBonusId);

        while(getBonuses->step()) {
            auto bonusType = static_cast<CostumeLevelBonusType>(getBonuses->columnInt(0));
            auto bonusValue = static_cast<int32_t>(getBonuses->columnInt(1));

            bonuses.apply(bonusType, bonusValue);
        }

        getBonuses->reset();
    }

    /*
     * Now, get any bonuses from stained glass.
     */
    auto queryStainedGlassBonuses = db().prepare(R"SQL(
        SELECT
            status_kind_type, status_calculation_type, effect_value
        FROM
            i_user_important_item,
            m_important_item USING (important_item_id),
            m_stained_glass ON stained_glass_id = external_reference_id,
            m_stained_glass_status_up_target_group USING (stained_glass_status_up_target_group_id),
            m_stained_glass_status_up_group USING (stained_glass_status_up_group_id)
        WHERE
            user_id = ? AND
            count > 0 AND
            important_item_type = 5 AND -- no enum, assuming 'stained glass'
            status_up_target_type = 1 AND -- CHARACTER
            target_value = ?
    )SQL");
    queryStainedGlassBonuses->bind(1, m_userId);
    queryStainedGlassBonuses->bind(2, character);
    while(queryStainedGlassBonuses->step()) {
        auto statusType = static_cast<StatusKindType>(queryStainedGlassBonuses->columnInt(0));
        auto statusCalculationType = static_cast<StatusCalculationType>(queryStainedGlassBonuses->columnInt(1));
        auto effectValue = static_cast<int32_t>(queryStainedGlassBonuses->columnInt(2));

        m_log.debug("including a stained glass bonus: status type %d, effect %d", static_cast<int32_t>(statusType), static_cast<int32_t>(effectValue));

        if(statusCalculationType != StatusCalculationType::ADD) {
            throw std::runtime_error("unsupported status caluclation type");
        }

        bonuses.applyKind(statusType, effectValue);
    }
    queryStainedGlassBonuses->reset();

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

auto UserContext::queryCharacterBoardReleaseStatus(int32_t characterBoardId) -> CharacterBoardReleaseStatus  {
    auto queryReleaseStatus = db().prepare(R"SQL(
        SELECT
            panel_release_bit1, panel_release_bit2, panel_release_bit3, panel_release_bit4
        FROM i_user_character_board
        WHERE
            user_id = ? AND
            character_board_id = ?
    )SQL");
    queryReleaseStatus->bind(1, m_userId);
    queryReleaseStatus->bind(2, characterBoardId);

    std::array<uint32_t, 4> releasedPanels{ 0, 0, 0, 0 };
    /*
     * For bug compatibility, we need to store uint32_t bitmasks as int32_t's,
     * i.e. make values with the highest bit go negative.
     */

    if(queryReleaseStatus->step()) {
        releasedPanels[0] = static_cast<uint32_t>(queryReleaseStatus->columnInt(0));
        releasedPanels[1] = static_cast<uint32_t>(queryReleaseStatus->columnInt(1));
        releasedPanels[2] = static_cast<uint32_t>(queryReleaseStatus->columnInt(2));
        releasedPanels[3] = static_cast<uint32_t>(queryReleaseStatus->columnInt(3));
    }

    return releasedPanels;
}

void UserContext::storeCharacterBoardReleaseStatus(int32_t characterBoardId, const CharacterBoardReleaseStatus &status) {
    /*
     * For bug compatibility, we need to store uint32_t bitmasks as int32_t's,
     * i.e. make values with the highest bit go negative.
     */
    auto storeReleaseStatus = db().prepare(R"SQL(
        INSERT INTO i_user_character_board (
            user_id,
            character_board_id,
            panel_release_bit1,
            panel_release_bit2,
            panel_release_bit3,
            panel_release_bit4
        ) VALUES (
            ?, ?, ?, ?, ?, ?
        ) ON CONFLICT (user_id, character_board_id) DO UPDATE SET
            panel_release_bit1 = excluded.panel_release_bit1,
            panel_release_bit2 = excluded.panel_release_bit2,
            panel_release_bit3 = excluded.panel_release_bit3,
            panel_release_bit4 = excluded.panel_release_bit4
    )SQL");
    storeReleaseStatus->bind(1, m_userId);
    storeReleaseStatus->bind(2, characterBoardId);
    storeReleaseStatus->bind(3, static_cast<int32_t>(status[0]));
    storeReleaseStatus->bind(4, static_cast<int32_t>(status[1]));
    storeReleaseStatus->bind(5, static_cast<int32_t>(status[2]));
    storeReleaseStatus->bind(6, static_cast<int32_t>(status[3]));
    storeReleaseStatus->exec();
}

void UserContext::splitCharacterBoardSortOrder(int32_t sortOrder, uint32_t &word, uint32_t &bit) {

    if(sortOrder < 1 || sortOrder > 4 * 32) {
        throw std::logic_error("character board panel sort order is out of range");
    }

    auto index = static_cast<uint32_t>(sortOrder - 1);
    word = index / 32;
    bit = index % 32;
}

bool UserContext::isCharacterBoardPanelReleased(const CharacterBoardReleaseStatus &status, int32_t sortOrder) {
    uint32_t word, bit;

    splitCharacterBoardSortOrder(sortOrder, word, bit);

    return (status[word] & (1U << bit)) != 0;
}

void UserContext::setCharacterBoardPanelReleased(CharacterBoardReleaseStatus &status, int32_t sortOrder, bool released) {
    uint32_t word, bit;

    splitCharacterBoardSortOrder(sortOrder, word, bit);

    if(released)
        status[word] |= (1U << bit);
    else
        status[word] &= ~(1U << bit);
}

void UserContext::releaseCharacterBoardPanel(int32_t panelId, CharacterBoardDeferredUpdate &update) {
    /*
     * Query the basic information about the panel:
     *   - which board it belongs to, and the index on the panel,
     *   - which character it belongs to.
     */
    auto queryPanelInfo = db().prepare(R"SQL(
        SELECT
            character_id,
            character_board_id,
            m_character_board_panel.sort_order AS panel_sort_order,
            character_board_panel_release_possession_group_id,
            character_board_panel_release_reward_group_id
        FROM
            m_character_board_panel,
            m_character_board USING (character_board_id),
            m_character_board_group USING (character_board_group_id),
            m_character_board_assignment USING (character_board_category_id)
        WHERE
            character_board_panel_id = ?
    )SQL");
    queryPanelInfo->bind(1, panelId);

    if(!queryPanelInfo->step())
        throw std::runtime_error("no such panel: " + std::to_string(panelId));

    auto characterId = queryPanelInfo->columnInt(0);
    auto characterBoardId = queryPanelInfo->columnInt(1);
    auto panelSortOrder = queryPanelInfo->columnInt(2);
    auto releasePossessionGroupId = queryPanelInfo->columnInt(3);
    auto releaseRewardGroupId = queryPanelInfo->columnInt(4);

    queryPanelInfo->reset();

    update.charactersToRecalculate.emplace(characterId);

    /*
     * Query the already released panels of this board.
     */
    auto alreadyReleased = queryCharacterBoardReleaseStatus(characterBoardId);

    if(isCharacterBoardPanelReleased(alreadyReleased, panelSortOrder))
        throw std::logic_error("the panel is already released");

    setCharacterBoardPanelReleased(alreadyReleased, panelSortOrder, true);
    storeCharacterBoardReleaseStatus(characterBoardId, alreadyReleased);

    /*
     * We don't bother checking the unlock conditions, since the client checks
     * them, and as usual, we trust the client.
     */

    if(releasePossessionGroupId != 0) {
        /*
         * Take the required possessions.
         */

        auto queryCost = db().prepare(R"SQL(
            SELECT
                possession_type,
                possession_id,
                count
            FROM m_character_board_panel_release_possession_group
            WHERE character_board_panel_release_possession_group_id = ?
            ORDER BY sort_order
        )SQL");

        queryCost->bind(1, releasePossessionGroupId);
        while(queryCost->step()) {
            auto possessionType = queryCost->columnInt(0);
            auto possessionId = queryCost->columnInt(1);
            auto count = queryCost->columnInt(2);
            takePossession(possessionType, possessionId, count);
        }
    }

    if(releaseRewardGroupId != 0) {

        /*
         * Give out the panel release rewards.
         */

        auto queryRewards = db().prepare(R"SQL(
            SELECT
                possession_type,
                possession_id,
                count
            FROM m_character_board_panel_release_reward_group
            WHERE character_board_panel_release_reward_group_id = ?
            ORDER BY sort_order
        )SQL");
        queryRewards->bind(1, releaseRewardGroupId);
        while(queryRewards->step()) {
            auto possessionType = queryRewards->columnInt(0);
            auto possessionId = queryRewards->columnInt(1);
            auto count = queryRewards->columnInt(2);
            givePossession(possessionType, possessionId, count);
        }
    }
}

void UserContext::finalizeCharacterBoardUpdate(const CharacterBoardDeferredUpdate &update) {
    auto queryBoards = db().prepare(R"SQL(
        SELECT character_board_id
        FROM
            m_character_board_assignment,
            m_character_board_group USING (character_board_category_id),
            m_character_board USING (character_board_group_id)
        WHERE
            character_id = ? AND
            character_board_group_type = ?
    )SQL");

    auto queryPanelEffects = db().prepare(R"SQL(
        SELECT
            m_character_board_panel.sort_order AS panel_sort_order,
            character_board_effect_type,
            character_board_effect_id,
            effect_value
        FROM
            m_character_board_panel,
            m_character_board_panel_release_effect_group USING (character_board_panel_release_effect_group_id)
        WHERE character_board_id = ?
        ORDER BY m_character_board_panel_release_effect_group.sort_order
    )SQL");

    auto queryStatusUpdate = db().prepare(R"SQL(
        SELECT
            character_board_status_up_type
        FROM
            m_character_board_status_up,
            m_character_board_effect_target_group USING (character_board_effect_target_group_id)
        WHERE
            character_board_status_up_id = ? AND
            group_index = 1 AND
            character_board_effect_target_type = 1 AND
            target_value = ?
    )SQL");

    auto queryAbility = db().prepare(R"SQL(
        SELECT
            ability_id
        FROM
            m_character_board_ability,
            m_character_board_effect_target_group USING (character_board_effect_target_group_id)
        WHERE
            character_board_ability_id = ? AND
            group_index = 1 AND
            character_board_effect_target_type = 1 AND
            target_value = ?
    )SQL");

    auto queryAbilityMaxLevel = db().prepare(
        "SELECT max_level FROM m_character_board_ability_max_level WHERE character_id = ? AND ability_id = ?"
    );

    auto insertCharacterBoardAbilityLevel = db().prepare(R"SQL(
        INSERT INTO i_user_character_board_ability (
            user_id,
            character_id,
            ability_id,
            level
        ) VALUES (
            ?, ?, ?, ?
        ) ON CONFLICT (user_id, character_id, ability_id) DO UPDATE SET
            level = excluded.level
    )SQL");

    auto insertCharacterBoardStatusUpdates = db().prepare(R"SQL(
        INSERT INTO i_user_character_board_status_up (
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

    for(auto characterId: update.charactersToRecalculate) {
        std::unordered_map<int32_t, int32_t> abilityLevels;

        /*
         * Bonus calculation effectively duplicates the costume level bonus
         * calculation, but this technically uses different enums.
         */
        AggregatedBonuses bonuses;

        /*
         * Query all boards.
         * TODO: character board groups have two types, BASIC and BIG_HUNT.
         * It's currently unclear what BIG_HUNT does, so we only take BASIC
         * in the account.
         */

        queryBoards->bind(1, characterId);
        queryBoards->bind(2, static_cast<int32_t>(CharacterBoardGroupType::BASIC));
        while(queryBoards->step()) {
            auto boardId = queryBoards->columnInt(0);

            auto boardCompletion = queryCharacterBoardReleaseStatus(boardId);

            /*
             * Query the effects of the panels of this board.
             */

            queryPanelEffects->bind(1, boardId);
            while(queryPanelEffects->step()) {
                auto panelSortOrder = queryPanelEffects->columnInt(0);
                if(!isCharacterBoardPanelReleased(boardCompletion, panelSortOrder))
                    continue;

                /*
                 * This panel is released, and is taking effect.
                 */

                auto effectType = static_cast<CharacterBoardEffectType>(queryPanelEffects->columnInt(1));
                auto effectId = queryPanelEffects->columnInt(2);
                auto effectValue = queryPanelEffects->columnInt(3);

                /*
                 * Most of m_character_board_effect_target_group seems to
                 * have static or predictable values. We assert that they
                 * are matching out expectations in the WHERE condition
                 * for the following queries, for both ABILITY and STATUS_UP.
                 */

                if(effectType == CharacterBoardEffectType::ABILITY) {
                    queryAbility->bind(1, effectId);
                    queryAbility->bind(2, characterId);
                    if(!queryAbility->step())
                        throw std::logic_error("no such character board ability: " + std::to_string(effectId));

                    auto abilityId = queryAbility->columnInt(0);
                    queryAbility->reset();

                    auto result = abilityLevels.emplace(abilityId, effectValue);
                    if(!result.second)
                        result.first->second += effectValue;

                } else if(effectType == CharacterBoardEffectType::STATUS_UP) {
                    queryStatusUpdate->bind(1, effectId);
                    queryStatusUpdate->bind(2, characterId);
                    if(!queryStatusUpdate->step())
                        throw std::logic_error("no such character board status update effect: " + std::to_string(effectId));

                    auto statusUpType = static_cast<CharacterBoardStatusUpType>(queryStatusUpdate->columnInt(0));
                    queryStatusUpdate->reset();

                    bonuses.apply(statusUpType, effectValue);

                } else {
                    throw std::runtime_error("unsupported character board panel effect type: " + std::to_string(static_cast<int32_t>(effectType)));
                }
            }

            queryPanelEffects->reset();
        }

        queryBoards->reset();

        for(const auto &abilityAndLevel : abilityLevels) {
            /*
             * Clamp each ability to its maximum level.
             */
            queryAbilityMaxLevel->bind(1, characterId);
            queryAbilityMaxLevel->bind(2, abilityAndLevel.first);
            if(!queryAbilityMaxLevel->step())
                throw std::runtime_error("unable to determine the max level of the character board ability " + std::to_string(abilityAndLevel.first));

            auto maxLevel = queryAbilityMaxLevel->columnInt(0);
            queryAbilityMaxLevel->reset();

            auto level = std::min<int32_t>(abilityAndLevel.second, maxLevel);

            /*
             * And store the calculated level.
             */

            insertCharacterBoardAbilityLevel->bind(1, m_userId);
            insertCharacterBoardAbilityLevel->bind(2, characterId);
            insertCharacterBoardAbilityLevel->bind(3, abilityAndLevel.first);
            insertCharacterBoardAbilityLevel->bind(4, level);
            insertCharacterBoardAbilityLevel->exec();
            insertCharacterBoardAbilityLevel->reset();
        }

        /*
         * Store the aggregated status update.
         */
        insertCharacterBoardStatusUpdates->bind(1, m_userId);
        insertCharacterBoardStatusUpdates->bind(2, characterId);
        insertCharacterBoardStatusUpdates->bind(3, static_cast<int32_t>(StatusCalculationType::ADD));
        insertCharacterBoardStatusUpdates->bind(4, bonuses.hp);
        insertCharacterBoardStatusUpdates->bind(5, bonuses.attack);
        insertCharacterBoardStatusUpdates->bind(6, bonuses.vitality);
        insertCharacterBoardStatusUpdates->bind(7, bonuses.agility);
        insertCharacterBoardStatusUpdates->bind(8, bonuses.criticalRatio);
        insertCharacterBoardStatusUpdates->bind(9, bonuses.criticalAttack);
        insertCharacterBoardStatusUpdates->exec();
        insertCharacterBoardStatusUpdates->reset();
    }
}

void UserContext::takePossession(int32_t possessionType, int32_t possessionId, int32_t count) {

    switch(static_cast<PossessionType>(possessionType)) {
        case PossessionType::CONSUMABLE_ITEM:
            consumeConsumableItem(possessionId, count);
            break;

        case PossessionType::MATERIAL:
            consumeMaterial(possessionId, count);
            break;

        default:
            throw std::runtime_error("takePossession: unsupported possession type " + std::to_string(possessionType));
    }


}

void UserContext::initGimmickSequenceSchedule() {
    auto evaluationTime = dataModel().serverTime();

    auto getSchedules = db().prepare(R"SQL(
        SELECT
            gimmick_sequence_schedule_id,
            first_gimmick_sequence_id,
            release_evaluate_condition_id
        FROM m_gimmick_sequence_schedule
        WHERE
            ? BETWEEN start_datetime AND end_datetime
    )SQL");

    auto checkScheduleActivity = db().prepare(R"SQL(
        SELECT 1 FROM i_user_gimmick_sequence
        WHERE
            user_id = ? AND
            gimmick_sequence_schedule_id = ?
    )SQL");

    getSchedules->bind(1, evaluationTime);
    while(getSchedules->step()) {
        auto sequenceScheduleId = getSchedules->columnInt(0);
        auto firstGimmickSequenceId = getSchedules->columnInt(1);
        auto releaseEvaluateConditionId = getSchedules->columnInt(2);

        m_log.debug("evaluating a gimmick sequence schedule for release: schedule ID %d, first gimmick sequence ID %d, release evaluate condition %d",
                    sequenceScheduleId, firstGimmickSequenceId, releaseEvaluateConditionId);

        if(!evaluateCondition(releaseEvaluateConditionId)) {
            m_log.debug("  not released due to a failed release condition");
            continue;
        }

        checkScheduleActivity->bind(1, m_userId);
        checkScheduleActivity->bind(2, sequenceScheduleId);
        bool active = checkScheduleActivity->step();
        checkScheduleActivity->reset();

        if(!active) {
            updateGimmickSequence(sequenceScheduleId, firstGimmickSequenceId);
        }
    }
}

void UserContext::updateGimmickSequence(int32_t sequenceScheduleId, int32_t gimmickSequenceId) {
    auto insert = db().prepare(R"SQL(
        INSERT INTO i_user_gimmick_sequence (
            user_id,
            gimmick_sequence_schedule_id,
            gimmick_sequence_id,
            is_gimmick_sequence_cleared,
            clear_datetime
        ) VALUES (
            ?, ?, ?, 0, 0
        ) ON CONFLICT (user_id, gimmick_sequence_schedule_id) DO UPDATE SET
            gimmick_sequence_id = excluded.gimmick_sequence_id,
            is_gimmick_sequence_cleared = excluded.is_gimmick_sequence_cleared,
            clear_datetime = excluded.clear_datetime
    )SQL");
    insert->bind(1, m_userId);
    insert->bind(2, sequenceScheduleId);
    insert->bind(3, gimmickSequenceId);
    insert->exec();
    insert->reset();

    auto getGimmicksOfSequence = db().prepare(R"SQL(
        SELECT
            m_gimmick_group.gimmick_id,
            release_evaluate_condition_id,
            gimmick_ornament_group_id
        FROM
            m_gimmick_sequence,
            m_gimmick_group USING (gimmick_group_id),
            m_gimmick USING (gimmick_id)
        WHERE
            gimmick_sequence_id = ?
        ORDER BY group_index
    )SQL");

    auto insertGimmick = db().prepare(R"SQL(
        INSERT INTO i_user_gimmick (
            user_id,
            gimmick_sequence_schedule_id,
            gimmick_sequence_id,
            gimmick_id,
            start_datetime
        ) VALUES (
            ?, ?, ?, ?,
            current_net_timestamp()
        ) ON CONFLICT DO NOTHING
    )SQL");

    auto getOrnaments = db().prepare(R"SQL(
        SELECT gimmick_ornament_index
        FROM m_gimmick_ornament
        WHERE
            gimmick_ornament_group_id = ?
    )SQL");

    auto insertOrnament = db().prepare(R"SQL(
        INSERT INTO i_user_gimmick_ornament_progress (
            user_id,
            gimmick_sequence_schedule_id,
            gimmick_sequence_id,
            gimmick_id,
            gimmick_ornament_index,
            base_datetime
        ) VALUES (
            ?, ?, ?, ?, ?,
            current_net_timestamp()
        ) ON CONFLICT DO NOTHING
    )SQL");

    getGimmicksOfSequence->bind(1, gimmickSequenceId);
    while(getGimmicksOfSequence->step()) {
        auto gimmickId = getGimmicksOfSequence->columnInt(0);
        auto gimmickReleaseConditionId = getGimmicksOfSequence->columnInt(1);
        auto ornamentGroupId = getGimmicksOfSequence->columnInt(2);

        /* TODO: is this check needed? */
#if 0
        if(!evaluateCondition(gimmickReleaseConditionId))
            continue;
#endif

        /*
            * This gimmick is released. Make sure that it's in the user table.
            */
        insertGimmick->bind(1, m_userId);
        insertGimmick->bind(2, sequenceScheduleId);
        insertGimmick->bind(3, gimmickSequenceId);
        insertGimmick->bind(4, gimmickId);
        insertGimmick->exec();
        insertGimmick->reset();

        getOrnaments->bind(1, ornamentGroupId);
        while(getOrnaments->step()) {
            auto ornamentIndex = getOrnaments->columnInt(0);

            /*
                * Make sure that the ornament is in the user table.
                */
            insertOrnament->bind(1, m_userId);
            insertOrnament->bind(2, sequenceScheduleId);
            insertOrnament->bind(3, gimmickSequenceId);
            insertOrnament->bind(4, gimmickId);
            insertOrnament->bind(5, ornamentIndex);
            insertOrnament->exec();
            insertOrnament->reset();

        }
        getOrnaments->reset();

    }
}

bool UserContext::evaluateCondition(int32_t evaluateConditionId, CollectedConditionRequirements *requirements) {
    /*
     * Null conditions are always true.
     */
    if(evaluateConditionId == 0)
        return true;

    auto getCondition = db().prepare(R"SQL(
        SELECT
            evaluate_condition_function_type,
            evaluate_condition_evaluate_type,
            evaluate_condition_value_group_id
        FROM
            m_evaluate_condition
        WHERE
            evaluate_condition_id = ?
    )SQL");
    getCondition->bind(1, evaluateConditionId);
    if(!getCondition->step())
        throw std::logic_error("no such evaluate condition");

    auto functionType = static_cast<EvaluateConditionFunctionType>(getCondition->columnInt(0));
    auto evaluateType = static_cast<EvaluateConditionEvaluateType>(getCondition->columnInt(1));
    auto evaluateConditionValueGroupId = getCondition->columnInt(2);
    getCondition->reset();

    std::vector<int32_t> values;
    auto getValues = db().prepare(R"SQL(
        SELECT group_index, value
        FROM m_evaluate_condition_value_group
        WHERE evaluate_condition_value_group_id = ?
        ORDER BY group_index
    )SQL");
    getValues->bind(1, evaluateConditionValueGroupId);
    while(getValues->step()) {
        auto index = getValues->columnInt(0);
        if(index != values.size() + 1)
            throw std::logic_error("inconsistent group_index in m_evaluate_condition_value_group");

        values.emplace_back(getValues->columnInt(1));
    }

    return evaluateCondition(functionType, evaluateType, values, requirements);
}

bool UserContext::evaluateCondition(
    EvaluateConditionFunctionType functionType,
    EvaluateConditionEvaluateType evaluateType,
    const std::vector<int32_t> &functionInputValues,
    CollectedConditionRequirements *requirements) {

    /*
     * Function types used: RECURSION, QUEST_CLEAR, MISSION_CLEAR, QUEST_MISSION_CLEAR, OTHER_GIMMICK_BIT_COUNT,
     * QUEST_SCENE_CHOICE, QUEST_NOT_CLEAR
     */

    switch(functionType) {
        case EvaluateConditionFunctionType::RECURSION:
        {
            if(evaluateType == EvaluateConditionEvaluateType::AND) {
                for(auto otherCondition: functionInputValues) {
                    if(!evaluateCondition(otherCondition, requirements))
                        return false;
                }

                return true;
            } else if(evaluateType == EvaluateConditionEvaluateType::OR) {
                for(auto otherCondition: functionInputValues) {
                    if(evaluateCondition(otherCondition, requirements))
                        return true;
                }

                return false;
            } else {
                /*
                 * No other evaluateTypes are used with RECURSION.
                 */
                throw std::runtime_error("unexpected evaluateType for RECURSION");
            }
        }

        case EvaluateConditionFunctionType::QUEST_CLEAR:
            if(evaluateType != EvaluateConditionEvaluateType::ID_CONTAIN)
                throw std::runtime_error("unexpected evaluateType for QUEST_CLEAR");

            if(functionInputValues.size() != 1)
                throw std::logic_error("exactly one value is expected for QUEST_CLEAR");

            return isQuestCleared(functionInputValues.front());


        case EvaluateConditionFunctionType::MISSION_CLEAR:
            if(evaluateType != EvaluateConditionEvaluateType::ID_CONTAIN)
                throw std::runtime_error("unexpected evaluateType for MISSION_CLEAR");

            if(functionInputValues.size() != 1)
                throw std::logic_error("exactly one value is expected for MISSION_CLEAR");

            if(requirements)
                requirements->missions.emplace(functionInputValues.front());

            return isMissionClear(functionInputValues.front());

        default:
            m_log.error("UserContext::evaluateCondition: unsupported functionType %d, used with evaluateType %d",
                        functionType, evaluateType);
            return false;
    }
}

bool UserContext::isMissionClear(int32_t missionId) {
    auto getMissionState = db().prepare(R"SQL(
        SELECT mission_progress_status_type FROM i_user_mission WHERE user_id = ? AND mission_id = ?
    )SQL");
    getMissionState->bind(1, m_userId);
    getMissionState->bind(2, missionId);

    if(getMissionState->step()) {
        auto state = static_cast<MissionProgressStatusType>(getMissionState->columnInt(0));
        return state == MissionProgressStatusType::CLEAR || state == MissionProgressStatusType::REWARD_RECEIVED;
    }

    return false;
}

void UserContext::unlockGimmick(int32_t gimmickSequenceScheduleId, int32_t gimmickSequenceId, int32_t gimmickId) {
    auto getGimmickInfo = db().prepare(
        "SELECT clear_evaluate_condition_id FROM m_gimmick WHERE gimmick_id = ?"
    );
    getGimmickInfo->bind(1, gimmickId);
    if(!getGimmickInfo->step())
        throw std::runtime_error("no such gimmick");

    auto conditionId = getGimmickInfo->columnInt(0);
    getGimmickInfo->reset();

    m_log.debug("unlockGimmick: sequence schedule %d, sequence %d, gimmick %d", gimmickSequenceScheduleId, gimmickSequenceId, gimmickId);

    CollectedConditionRequirements requirements;

    auto cleared = evaluateCondition(conditionId, &requirements);

    if(!cleared && !requirements.missions.empty()) {
        /*
         * HACK: this is a temporary hack, because we don't have missions implemented yet.
         * TODO: just 'start' missions here in this block, when missions *are* implemented
         */
        m_log.error("unlockGimmick: sequence schedule %d, sequence %d, gimmick %d: not cleared with missions referenced, completing them",
                     gimmickSequenceScheduleId, gimmickSequenceId, gimmickId);

        auto forceCompleteMission = db().prepare(R"SQL(
            INSERT INTO i_user_mission (
                user_id,
                mission_id,
                start_datetime,
                mission_progress_status_type,
                clear_datetime
            ) VALUES (
                ?,
                ?,
                current_net_timestamp(),
                ?,
                current_net_timestamp()

            ) ON CONFLICT (user_id, mission_id) DO UPDATE SET
                mission_progress_status_type = MAX(mission_progress_status_type, excluded.mission_progress_status_type),
                clear_datetime = CASE clear_datetime WHEN 0 THEN excluded.clear_datetime ELSE clear_datetime END
        )SQL");
        for(auto mission: requirements.missions) {
            forceCompleteMission->bind(1, m_userId);
            forceCompleteMission->bind(2, mission);
            forceCompleteMission->bind(3, static_cast<int32_t>(MissionProgressStatusType::CLEAR));
            forceCompleteMission->exec();
            forceCompleteMission->reset();
        }

        /*
         * Don't reevaluate (or it'll confuse the client). The user should
         * attempt unlocking the second ime.
         */
    }

    /*
     * Should 'is_unlocked' be always true?
     */
    auto unlock = db().prepare(R"SQL(
        INSERT INTO i_user_gimmick_unlock (
            user_id,
            gimmick_sequence_schedule_id,
            gimmick_sequence_id,
            gimmick_id,
            is_unlocked
        ) VALUES (
            ?, ?, ?, ?, ?
        ) ON CONFLICT DO UPDATE SET is_unlocked = excluded.is_unlocked
    )SQL");
    unlock->bind(1, m_userId);
    unlock->bind(2, gimmickSequenceScheduleId);
    unlock->bind(3, gimmickSequenceId);
    unlock->bind(4, gimmickId);
    unlock->bind(5, static_cast<int32_t>(cleared));
    unlock->exec();
}

void UserContext::updateGimmickProgress(int32_t gimmickSequenceScheduleId, int32_t gimmickSequenceId, int32_t gimmickId, int32_t gimmickOrnamentIndex,
                            int32_t progressValueBit,
                            google::protobuf::RepeatedPtrField<apb::api::gimmick::GimmickReward> *gimmickOrnamentReward,
                            bool &sequenceCleared,
                            google::protobuf::RepeatedPtrField<apb::api::gimmick::GimmickReward> *gimmickSequenceClearReward) {

    sequenceCleared = false;

    m_log.debug("updating gimmick progress: schedule %d sequence %d gimmick %d ornament %d, progress is now %d",
                gimmickSequenceScheduleId, gimmickSequenceId, gimmickId, gimmickOrnamentIndex, progressValueBit);

    auto getGimmickOrnamentInfo = db().prepare(R"SQL(
        SELECT
            count
        FROM
            m_gimmick,
            m_gimmick_ornament USING (gimmick_ornament_group_id)
        WHERE
            gimmick_id = ? AND
            gimmick_ornament_index = ?
    )SQL");
    getGimmickOrnamentInfo->bind(1, gimmickId);
    getGimmickOrnamentInfo->bind(2, gimmickOrnamentIndex);
    if(!getGimmickOrnamentInfo->step())
        throw std::logic_error("no such gimmick ornament");

    auto gimmickOrnamentCount = getGimmickOrnamentInfo->columnInt(0);
    getGimmickOrnamentInfo->reset();

    auto updateGimmickOrnamentState = db().prepare(R"SQL(
        UPDATE i_user_gimmick_ornament_progress SET progress_value_bit = ?
        WHERE
            user_id = ? AND
            gimmick_sequence_schedule_id = ? AND
            gimmick_sequence_id = ? AND
            gimmick_id = ? AND
            gimmick_ornament_index = ?
        RETURNING 1
    )SQL");
    updateGimmickOrnamentState->bind(1, progressValueBit);
    updateGimmickOrnamentState->bind(2, m_userId);
    updateGimmickOrnamentState->bind(3, gimmickSequenceScheduleId);
    updateGimmickOrnamentState->bind(4, gimmickSequenceId);
    updateGimmickOrnamentState->bind(5, gimmickId);
    updateGimmickOrnamentState->bind(6, gimmickOrnamentIndex);
    if(!updateGimmickOrnamentState->step())
        throw std::logic_error("the gimmick ornament is not active");
    updateGimmickOrnamentState->reset();

    /*
     * Check if all ornaments of this gimmick are now clear.
     * TODO: it looks that gimmicks with ornaments with the count 0 of are supposed to *not be clearable*?
     * The client code needs more investigation.
     */
    auto checkGimmickClear = db().prepare(R"SQL(
        SELECT
            COALESCE(MIN(CASE WHEN count == 0 THEN 0 ELSE COALESCE(progress_value_bit, 0) == ((1 << count) - 1) END), 0)
        FROM
            m_gimmick,
            m_gimmick_ornament USING (gimmick_ornament_group_id)
            LEFT JOIN i_user_gimmick_ornament_progress ON
                user_id = ? AND
                gimmick_sequence_schedule_id = ? AND
                gimmick_sequence_id = ? AND
                i_user_gimmick_ornament_progress.gimmick_id = m_gimmick.gimmick_id AND
                i_user_gimmick_ornament_progress.gimmick_ornament_index = m_gimmick_ornament.gimmick_ornament_index
        WHERE
            m_gimmick.gimmick_id = ?
    )SQL");
    checkGimmickClear->bind(1, m_userId);
    checkGimmickClear->bind(2, gimmickSequenceScheduleId);
    checkGimmickClear->bind(3, gimmickSequenceId);
    checkGimmickClear->bind(4, gimmickId);
    if(!checkGimmickClear->step())
        throw std::logic_error("unexpected empty set");

    auto gimmickClear = checkGimmickClear->columnInt(0);
    if(!gimmickClear) {
        /*
         * If this gimmick is not clear yet, that's all.
         */

        return;
    }

    auto clearGimmick = db().prepare(R"SQL(
        UPDATE i_user_gimmick SET is_gimmick_cleared = 1
        WHERE
            user_id = ? AND
            gimmick_sequence_schedule_id = ? AND
            gimmick_sequence_id = ? AND
            gimmick_id = ? AND
            is_gimmick_cleared = 0
        RETURNING 1
    )SQL");
    clearGimmick->bind(1, m_userId);
    clearGimmick->bind(2, gimmickSequenceScheduleId);
    clearGimmick->bind(3, gimmickSequenceId);
    clearGimmick->bind(4, gimmickId);
    if(!clearGimmick->step()) {
        /*
         * If this gimmick wasn't *newly* cleared, then that's all.
         */

        return;
    }

    m_log.debug("gimmick schedule %d sequence %d gimmick %d is now clear", gimmickSequenceScheduleId, gimmickSequenceId, gimmickId);

    /*
     * This gimmick is now cleared.
     * TODO: we are likely supposed to issue gimmick clear rewards (there are
     * API provisions for that), but there doesn't seem to be a table for it.
     */

    /*
     * Check if all gimmicks of the sequence are now clear.
     * There's 'gimmick_sequence_clear_condition_type' in m_gimmick_sequence,
     * however, it seems to be always 1 and there doesn't seem to be a
     * corresponding enum in the client. Currently, we assume that gimmick
     * sequences should be cleared once all gimmicks are cleared.
     *
     * NOTE: the client takes care of checking clear_evaluate_condition_id on
     * the gimmick, we won't get this API call if the gimmick should *not* be
     * cleared.
     */
    auto checkSequenceClear = db().prepare(R"SQL(
        SELECT
            COALESCE(MIN(COALESCE(is_gimmick_cleared, 0)), 0)
        FROM
            m_gimmick_sequence,
            m_gimmick_group USING (gimmick_group_id),
            m_gimmick USING (gimmick_id)
            LEFT JOIN i_user_gimmick ON
                user_id = ? AND
                gimmick_sequence_schedule_id = ? AND
                i_user_gimmick.gimmick_sequence_id = m_gimmick_sequence.gimmick_sequence_id AND
                i_user_gimmick.gimmick_id = m_gimmick.gimmick_id
        WHERE m_gimmick_sequence.gimmick_sequence_id = ?
    )SQL");
    checkSequenceClear->bind(1, m_userId);
    checkSequenceClear->bind(2, gimmickSequenceScheduleId);
    checkSequenceClear->bind(3, gimmickSequenceId);
    if(!checkSequenceClear->step())
        throw std::logic_error("unexpected empty set");

    auto gimmickSequenceClear = checkSequenceClear->columnInt(0);
    if(!gimmickSequenceClear) {
        /*
         * If this gimmick sequence is not clear yet, that's all.
         */

        return;
    }

    /*
     * Clear the sequence if it's not clear yet.
     */
    auto clearSequence = db().prepare(R"SQL(
        UPDATE i_user_gimmick_sequence SET
            is_gimmick_sequence_cleared = 1,
            clear_datetime = current_net_timestamp()
        WHERE
            user_id = ? AND
            gimmick_sequence_schedule_id = ? AND
            gimmick_sequence_id = ? AND
            is_gimmick_sequence_cleared = 0
        RETURNING 1
    )SQL");
    clearSequence->bind(1, m_userId);
    clearSequence->bind(2, gimmickSequenceScheduleId);
    clearSequence->bind(3, gimmickSequenceId);
    if(!clearSequence->step()) {
        /*
         * If this sequence wasn't *newly* cleared, then that's all.
         */

        return;
    }

    m_log.debug("gimmick schedule %d sequence %d is now clear", gimmickSequenceScheduleId, gimmickSequenceId);

    sequenceCleared = true;

    /*
     * Get the rewards for clearing this gimmick sequence, and issue them.
     */
    auto getRewards = db().prepare(R"SQL(
        SELECT
            possession_type,
            possession_id,
            count
        FROM
            m_gimmick_sequence,
            m_gimmick_sequence_reward_group USING (gimmick_sequence_reward_group_id)
        WHERE
            gimmick_sequence_id = ?
    )SQL");
    getRewards->bind(1, gimmickSequenceId);
    while(getRewards->step()) {
        auto possessionType = getRewards->columnInt(0);
        auto possessionId = getRewards->columnInt(1);
        auto count = getRewards->columnInt(2);

        givePossession(possessionType, possessionId, count);
        auto reward = gimmickSequenceClearReward->Add();
        reward->set_possession_type(possessionType);
        reward->set_possession_id(possessionId);
        reward->set_count(count);
    }
}

void UserContext::weaponEvolve(const std::string &weaponUUID) {
    auto getStatus = db().prepare(R"SQL(
        SELECT
            weapon_evolution_material_group_id,
            weapon_evolution_grant_possession_group_id,
            COALESCE(
                m_weapon_specific_enhance.evolution_cost_numerical_function_id,
                m_weapon_rarity.evolution_cost_numerical_function_id
            ),
            m_weapon_evolution_group.evolution_order,
            next_evolution.weapon_id
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_evolution_group USING (weapon_id),
            m_weapon_evolution_group AS next_evolution ON
                next_evolution.weapon_evolution_group_id = m_weapon_evolution_group.weapon_evolution_group_id AND
                next_evolution.evolution_order = m_weapon_evolution_group.evolution_order + 1,
            m_weapon_rarity USING (rarity_type) LEFT JOIN
            m_weapon_specific_enhance USING (weapon_specific_enhance_id)
        WHERE
            user_id = ? AND
            user_weapon_uuid = ?
    )SQL");
    getStatus->bind(1, m_userId);
    getStatus->bind(2, weaponUUID);
    if(!getStatus->step())
        throw std::logic_error("no such weapon, no evolution relation, or no next evolution");

    auto weaponEvolutionMaterialGroupId = getStatus->columnInt(0);
    auto weaponEvolutionGrantPossessionGroupId = getStatus->columnInt(1);
    auto evolutionCostNumericalFunctionId = getStatus->columnInt(2);
    auto currentEvolutionOrder = getStatus->columnInt(3);
    auto nextWeaponId = getStatus->columnInt(4);

    if(weaponEvolutionGrantPossessionGroupId != 0) {
        /*
         * The corresponding table seem to absent and all weapons have this
         * value as zero in the database.
         */
        throw std::logic_error("unexpected non-zero weapon_evolution_grant_possession_group_id");
    }

    getStatus->reset();

    size_t numberOfKindsOfMaterialsUsed = 0;

    auto getMaterial = db().prepare(R"SQL(
        SELECT material_id, count
        FROM m_weapon_evolution_material_group
        WHERE weapon_evolution_material_group_id = ?
        ORDER BY sort_order
    )SQL");
    getMaterial->bind(1, weaponEvolutionMaterialGroupId);
    while(getMaterial->step()) {
        auto materialId = getMaterial->columnInt(0);
        auto count = getMaterial->columnInt(1);

        consumeMaterial(materialId, count);
        numberOfKindsOfMaterialsUsed++;
    }

    auto cost = evaluateNumericalFunction(evolutionCostNumericalFunctionId, numberOfKindsOfMaterialsUsed);

    m_log.debug("weaponEvolve(%s): current evolution order: %d, next evolution weapon ID: %d, cost: %d\n",
                weaponUUID.c_str(), currentEvolutionOrder, nextWeaponId,
                cost);

    consumeConsumableItem(consumableItemIdForGold(), cost);

    auto evolveWeapon = db().prepare("UPDATE i_user_weapon SET weapon_id = ? WHERE user_id = ? AND user_weapon_uuid = ?");
    evolveWeapon->bind(1, nextWeaponId);
    evolveWeapon->bind(2, m_userId);
    evolveWeapon->bind(3, weaponUUID);
    evolveWeapon->exec();

    populateWeaponSkillsAndAbilities(weaponUUID);
    populateWeaponNote(weaponUUID);

    giveUserWeaponExperience(weaponUUID, 0, 0);
    updateWeaponUnlockedStory(nextWeaponId);
}

void UserContext::populateWeaponSkillsAndAbilities(const std::string &weaponUUID) {

    /*
     * Get the currently defined skill and ability slots.
     */
    std::unordered_set<int32_t> existingSkillSlots, existingAbilitySlots;

    auto getSkills = db().prepare("SELECT slot_number FROM i_user_weapon_skill WHERE user_id = ? AND user_weapon_uuid = ?");
    getSkills->bind(1, m_userId);
    getSkills->bind(2, weaponUUID);
    while(getSkills->step()) {
        existingSkillSlots.emplace(getSkills->columnInt(0));
    }
    getSkills->reset();

    auto getAbilities = db().prepare("SELECT slot_number FROM i_user_weapon_ability WHERE user_id = ? AND user_weapon_uuid = ?");
    getAbilities->bind(1, m_userId);
    getAbilities->bind(2, weaponUUID);
    while(getAbilities->step()) {
        existingAbilitySlots.emplace(getAbilities->columnInt(0));
    }
    getAbilities->reset();

    /*
     * Get the skill and ability slots this weapon is supposed to have.
     */
    auto getDefinedSkills = db().prepare(R"SQL(
        SELECT slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_skill_group USING (weapon_skill_group_id)
        WHERE
            user_id = ? AND
            user_weapon_uuid = ?
    )SQL");
    getDefinedSkills->bind(1, m_userId);
    getDefinedSkills->bind(2, weaponUUID);

    auto createSkillSlot = db().prepare(R"SQL(
        INSERT INTO i_user_weapon_skill (
            user_id,
            user_weapon_uuid,
            slot_number
        ) VALUES (
            ?, ?, ?
        )
    )SQL");
    createSkillSlot->bind(1, m_userId);
    createSkillSlot->bind(2, weaponUUID);

    auto deleteSkillSlot = db().prepare(R"SQL(
        DELETE FROM i_user_weapon_skill WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            slot_number = ?
    )SQL");
    deleteSkillSlot->bind(1, m_userId);
    deleteSkillSlot->bind(2, weaponUUID);

    while(getDefinedSkills->step()) {
        auto slotNumber = getDefinedSkills->columnInt(0);

        if(existingSkillSlots.erase(slotNumber) == 0) {
            /*
             * We don't already have that skill slot, so create it.
             */
            createSkillSlot->bind(3, slotNumber);
            createSkillSlot->exec();
            createSkillSlot->reset();
        }
    }

    getDefinedSkills->reset();

    /*
     * Any remaining skill slots were removed, and we need to delete them.
     */
    for(auto existing: existingSkillSlots) {
        deleteSkillSlot->bind(3, existing);
        deleteSkillSlot->exec();
    }

    /*
     * Now, the same for abilities.
     */
    auto getDefinedAbilities = db().prepare(R"SQL(
        SELECT slot_number
        FROM
            i_user_weapon,
            m_weapon USING (weapon_id),
            m_weapon_ability_group USING (weapon_ability_group_id)
        WHERE
            user_id = ? AND
            user_weapon_uuid = ?
    )SQL");
    getDefinedAbilities->bind(1, m_userId);
    getDefinedAbilities->bind(2, weaponUUID);

    auto createAbilitySlot = db().prepare(R"SQL(
        INSERT INTO i_user_weapon_ability (
            user_id,
            user_weapon_uuid,
            slot_number
        ) VALUES (
            ?, ?, ?
        )
    )SQL");
    createAbilitySlot->bind(1, m_userId);
    createAbilitySlot->bind(2, weaponUUID);

    auto deleteAbilitySlot = db().prepare(R"SQL(
        DELETE FROM i_user_weapon_ability WHERE
            user_id = ? AND
            user_weapon_uuid = ? AND
            slot_number = ?
    )SQL");
    deleteAbilitySlot->bind(1, m_userId);
    deleteAbilitySlot->bind(2, weaponUUID);

    while(getDefinedAbilities->step()) {
        auto slotNumber = getDefinedAbilities->columnInt(0);

        if(existingAbilitySlots.erase(slotNumber) == 0) {
            /*
             * We don't already have that ability slot, so create it.
             */
            createAbilitySlot->bind(3, slotNumber);
            createAbilitySlot->exec();
            createAbilitySlot->reset();
        }
    }

    getDefinedAbilities->reset();

    /*
     * Any remaining ability slots were removed, and we need to delete them.
     */
    for(auto existing: existingAbilitySlots) {
        deleteAbilitySlot->bind(3, existing);
        deleteAbilitySlot->exec();
    }
}

void UserContext::populateWeaponNote(const std::string &weaponUUID) {

    auto getWeaponIDAndLevel = db().prepare("SELECT weapon_id, level FROM i_user_weapon WHERE user_id = ? AND user_weapon_uuid = ?");
    getWeaponIDAndLevel->bind(1, m_userId);
    getWeaponIDAndLevel->bind(2, weaponUUID);
    if(!getWeaponIDAndLevel->step())
        throw std::logic_error("no such weapon");

    auto weaponID = getWeaponIDAndLevel->columnInt(0);
    auto level = getWeaponIDAndLevel->columnInt(1);
    getWeaponIDAndLevel->reset();

    auto note = db().prepare(R"SQL(
        INSERT INTO i_user_weapon_note (
            user_id,
            weapon_id,
            max_level,
            first_acquisition_datetime
        ) VALUES (
            ?,
            ?,
            ?,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, weapon_id) DO UPDATE SET
            max_level = MAX(max_level, excluded.max_level)
    )SQL");

    note->bind(1, m_userId);
    note->bind(2, weaponID);
    note->bind(3, level);
    note->exec();
}

void UserContext::registerContentsStoryPlayed(int32_t contentsStoryId) {
    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_contents_story (
            user_id,
            contents_story_id,
            play_datetime
        ) VALUES (
            ?,
            ?,
            current_net_timestamp()
        ) ON CONFLICT (user_id, contents_story_id) DO UPDATE SET
            play_datetime = excluded.play_datetime
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, contentsStoryId);
    query->exec();
}

void UserContext::weaponAwaken(const std::string &weaponUUID) {
    auto getWeaponId = db().prepare("SELECT weapon_id FROM i_user_weapon WHERE user_id = ? AND user_weapon_uuid = ?");
    getWeaponId->bind(1, m_userId);
    getWeaponId->bind(2, weaponUUID);
    if(!getWeaponId->step())
        throw std::runtime_error("no such weapon");

    auto weaponId = getWeaponId->columnInt(0);
    getWeaponId->reset();

    auto getAwakenInfo = db().prepare(R"SQL(
        SELECT weapon_awaken_material_group_id, consume_gold
        FROM m_weapon_awaken
        WHERE weapon_id = ?
    )SQL");
    getAwakenInfo->bind(1, weaponId);
    if(!getAwakenInfo->step())
        throw std::runtime_error("the weapon has no awakening setup");

    auto materialGroupId = getAwakenInfo->columnInt(0);
    auto consumeGold = getAwakenInfo->columnInt(1);
    getAwakenInfo->reset();

    consumeConsumableItem(consumableItemIdForGold(), consumeGold);

    auto getMaterials = db().prepare(R"SQL(
        SELECT material_id, count
        FROM m_weapon_awaken_material_group
        WHERE weapon_awaken_material_group_id = ?
        ORDER BY sort_order
    )SQL");
    getMaterials->bind(1, materialGroupId);
    while(getMaterials->step()) {
        auto materialId = getMaterials->columnInt(0);
        auto count = getMaterials->columnInt(1);

        consumeMaterial(materialId, count);
    }
    getMaterials->reset();

    auto recordAwakening = db().prepare("INSERT INTO i_user_weapon_awaken (user_id, user_weapon_uuid) VALUES (?, ?)");
    recordAwakening->bind(1, m_userId);
    recordAwakening->bind(2, weaponUUID);
    recordAwakening->exec();

    /*
     * Reevaluate the level, since the awakening may increase the level limit.
     */
    giveUserWeaponExperience(weaponUUID, 0, 0);
}


void UserContext::costumeAwaken(const std::string &costumeUUID,
                                    const google::protobuf::Map<int32_t, int32_t> &materialsToUse) {

/*
 * TODO: we probably could validate the material set, but it doesn't really matter.
 */

    auto getCostumeStatus = db().prepare(
        "SELECT costume_id, awaken_count FROM i_user_costume WHERE user_id = ? AND user_costume_uuid = ?"
    );
    getCostumeStatus->bind(1, m_userId);
    getCostumeStatus->bind(2, costumeUUID);
    if(!getCostumeStatus->step())
        throw std::runtime_error("no such costume");

    auto costumeId = getCostumeStatus->columnInt(0);
    auto currentAwakenCount = getCostumeStatus->columnInt(1);
    getCostumeStatus->reset();

    if(currentAwakenCount >= getIntConfig("COSTUME_AWAKEN_AVAILABLE_COUNT"))
        throw std::runtime_error("already at the maximum count of awakenings");

    auto awakeningStep = currentAwakenCount + 1;

    auto getAwakenCostInformation = db().prepare(R"SQL(
        SELECT
            gold
        FROM
            m_costume_awaken,
            m_costume_awaken_price_group using (costume_awaken_price_group_id)
        WHERE costume_id = ? AND awaken_step_lower_limit <= ?
        ORDER BY awaken_step_lower_limit DESC
    )SQL");
    getAwakenCostInformation->bind(1, costumeId);
    getAwakenCostInformation->bind(2, awakeningStep);
    if(!getAwakenCostInformation->step())
        throw std::runtime_error("unable to retrieve the awakening cost information");

    auto awakenCostGold = getAwakenCostInformation->columnInt(0);

    int32_t materialCount = 0;

    for(const auto &pair: materialsToUse) {
        consumeMaterial(pair.first, pair.second);
    }

    consumeConsumableItem(consumableItemIdForGold(), awakenCostGold);

    auto updateAwakenCount = db().prepare(R"SQL(
        UPDATE i_user_costume SET
            awaken_count = awaken_count + 1
        WHERE
            user_id = ? AND
            user_costume_uuid = ?
    )SQL");
    updateAwakenCount->bind(1, m_userId);
    updateAwakenCount->bind(2, costumeUUID);
    updateAwakenCount->exec();

    /*
     * Get status effects that apply *up to the new level*, aggregate them, and store.
     */
    auto getStatusEffects = db().prepare(R"SQL(
        SELECT
            status_kind_type,
            status_calculation_type,
            effect_value
        FROM
            m_costume_awaken,
            m_costume_awaken_effect_group USING (costume_awaken_effect_group_id),
            m_costume_awaken_status_up_group ON m_costume_awaken_status_up_group.costume_awaken_status_up_group_id = m_costume_awaken_effect_group.costume_awaken_effect_id
        WHERE
            costume_id = ? AND
            awaken_step <= ? AND
            costume_awaken_effect_type = ?
        ORDER BY sort_order
    )SQL");
    getStatusEffects->bind(1, costumeId);
    getStatusEffects->bind(2, awakeningStep);
    getStatusEffects->bind(3, static_cast<int32_t>(CostumeAwakenEffectType::STATUS_UP));

    std::optional<AggregatedBonuses> additive, multiplicative;

    while(getStatusEffects->step()) {
        auto statusKindType = static_cast<StatusKindType>(getStatusEffects->columnInt(0));
        auto statusCalculationType = static_cast<StatusCalculationType>(getStatusEffects->columnInt(1));
        auto effectValue = getStatusEffects->columnInt(2);

        if(statusCalculationType == StatusCalculationType::ADD) {
            if(!additive.has_value())
                additive.emplace();

            switch(statusKindType) {
                case StatusKindType::HP:
                    additive->hp += effectValue;
                    break;

                case StatusKindType::ATTACK:
                    additive->attack += effectValue;
                    break;

                case StatusKindType::VITALITY:
                    additive->vitality += effectValue;
                    break;

                case StatusKindType::AGILITY:
                    additive->agility += effectValue;
                    break;

                case StatusKindType::CRITICAL_RATIO:
                    additive->criticalRatio += effectValue;
                    break;

                case StatusKindType::CRITICAL_ATTACK:
                    additive->criticalAttack += effectValue;
                    break;

                default:
                    throw std::runtime_error("unsupported StatusKindType");
            }
        } else if(statusCalculationType == StatusCalculationType::MULTIPLY) {
            if(!multiplicative.has_value()) {
                auto &def = multiplicative.emplace();
#if 0
                def.hp = 100;
                def.attack = 100;
                def.vitality = 100;
                def.agility = 100;
                def.criticalRatio = 100;
                def.criticalAttack = 100;
#endif
            }

            switch(statusKindType) {
#if 0
                case StatusKindType::HP:
                    multiplicative->hp = multiplicative->hp * effectValue / 100;
                    break;

                case StatusKindType::ATTACK:
                    multiplicative->attack = multiplicative->attack * effectValue / 100;
                    break;

                case StatusKindType::VITALITY:
                    multiplicative->vitality = multiplicative->vitality * effectValue / 100;
                    break;

                case StatusKindType::AGILITY:
                    multiplicative->agility = multiplicative->agility * effectValue / 100;
                    break;

                case StatusKindType::CRITICAL_RATIO:
                    multiplicative->criticalRatio = multiplicative->criticalRatio * effectValue / 100;
                    break;

                case StatusKindType::CRITICAL_ATTACK:
                    multiplicative->criticalAttack = multiplicative->criticalAttack * effectValue / 100;
                    break;
#endif

                case StatusKindType::HP:
                    multiplicative->hp += effectValue;
                    break;

                case StatusKindType::ATTACK:
                    multiplicative->attack += effectValue;
                    break;

                case StatusKindType::VITALITY:
                    multiplicative->vitality += effectValue;
                    break;

                case StatusKindType::AGILITY:
                    multiplicative->agility += effectValue;
                    break;

                case StatusKindType::CRITICAL_RATIO:
                    multiplicative->criticalRatio += effectValue;
                    break;

                case StatusKindType::CRITICAL_ATTACK:
                    multiplicative->criticalAttack += effectValue;
                    break;

                default:
                    throw std::runtime_error("unsupported StatusKindType");
            }
        } else {
            throw std::runtime_error("unsupported StatusCalculationType");
        }

    }
    getStatusEffects->reset();

    auto storeStatusEffect = db().prepare(R"SQL(
        INSERT INTO i_user_costume_awaken_status_up (
            user_id,
            user_costume_uuid,
            status_calculation_type,
            hp,
            attack,
            vitality,
            agility,
            critical_ratio,
            critical_attack
        ) VALUES (
            ?, ?, ?, ?, ?, ?, ?, ?, ?
        ) ON CONFLICT (user_id, user_costume_uuid, status_calculation_type) DO UPDATE SET
            hp = excluded.hp,
            attack = excluded.attack,
            vitality = excluded.vitality,
            agility = excluded.agility,
            critical_ratio = excluded.critical_ratio,
            critical_attack = excluded.critical_attack
    )SQL");
    storeStatusEffect->bind(1, m_userId);
    storeStatusEffect->bind(2, costumeUUID);
    if(additive.has_value()) {
        storeStatusEffect->bind(3, static_cast<int32_t>(StatusCalculationType::ADD));
        storeStatusEffect->bind(4, additive->hp);
        storeStatusEffect->bind(5, additive->attack);
        storeStatusEffect->bind(6, additive->vitality);
        storeStatusEffect->bind(7, additive->agility);
        storeStatusEffect->bind(8, additive->criticalRatio);
        storeStatusEffect->bind(9, additive->criticalAttack);
        storeStatusEffect->exec();
        storeStatusEffect->reset();
    }

    if(multiplicative.has_value()) {
        storeStatusEffect->bind(3, static_cast<int32_t>(StatusCalculationType::MULTIPLY));
        storeStatusEffect->bind(4, multiplicative->hp);
        storeStatusEffect->bind(5, multiplicative->attack);
        storeStatusEffect->bind(6, multiplicative->vitality);
        storeStatusEffect->bind(7, multiplicative->agility);
        storeStatusEffect->bind(8, multiplicative->criticalRatio);
        storeStatusEffect->bind(9, multiplicative->criticalAttack);
        storeStatusEffect->exec();
        storeStatusEffect->reset();
    }

    /*
     * It's currently believed that the ability effects require no server-side processing.
     */

    /*
     * Process the 'item acquire' effects *for the new level* only.
     */
    auto getItemEffects = db().prepare(R"SQL(
        SELECT
            possession_type,
            possession_id,
            count
        FROM
            m_costume_awaken,
            m_costume_awaken_effect_group USING (costume_awaken_effect_group_id),
            m_costume_awaken_item_acquire ON m_costume_awaken_item_acquire.costume_awaken_item_acquire_id = m_costume_awaken_effect_group.costume_awaken_effect_id
        WHERE
            costume_id = ? AND
            awaken_step = ? AND
            costume_awaken_effect_type = ?
    )SQL");
    getItemEffects->bind(1, costumeId);
    getItemEffects->bind(2, awakeningStep);
    getItemEffects->bind(3, static_cast<int32_t>(CostumeAwakenEffectType::ITEM_ACQUIRE));

    while(getItemEffects->step()) {
        auto possessionType = getItemEffects->columnInt(0);
        auto possessionId = getItemEffects->columnInt(1);
        auto count = getItemEffects->columnInt(2);

        givePossession(possessionType, possessionId, count);
    }
    getItemEffects->reset();
}


void UserContext::moveSideStoryQuest(int32_t sideStoryQuestId) {
    leaveSpecialStates();

    /*
     * Check if we have a record for this side story quest.
     */

    auto querySideStory = db().prepare(R"SQL(
        SELECT
            head_side_story_quest_scene_id,
            side_story_quest_state_type
        FROM i_user_side_story_quest
        WHERE
            user_id = ? AND
            side_story_quest_id = ?
    )SQL");
    querySideStory->bind(1, m_userId);
    querySideStory->bind(2, sideStoryQuestId);

    int32_t currentQuestSceneId;

    if(querySideStory->step()) {
        currentQuestSceneId = querySideStory->columnInt(0);
    } else {

        /*
         * We don't have a record, so we need to initialize.
         */

        auto getFirstScene = db().prepare(R"SQL(
            SELECT
                side_story_quest_scene_id
            FROM
                m_side_story_quest_scene
            WHERE
                side_story_quest_id = ?
            ORDER BY sort_order
            LIMIT 1
        )SQL");
        getFirstScene->bind(1, sideStoryQuestId);
        if(!getFirstScene->step()) {
            throw std::runtime_error("no such side story quest, or no scenes");
        }

        currentQuestSceneId = getFirstScene->columnInt(0);

        auto insertQuest = db().prepare(R"SQL(
            INSERT INTO i_user_side_story_quest (
                user_id,
                side_story_quest_id,
                head_side_story_quest_scene_id,
                side_story_quest_state_type
            ) VALUES (
                ?, ?, ?, ?
            )
        )SQL");
        insertQuest->bind(1, m_userId);
        insertQuest->bind(2, sideStoryQuestId);
        insertQuest->bind(3, currentQuestSceneId);
        insertQuest->bind(4, static_cast<int32_t>(QuestStateType::InProgress));
        insertQuest->exec();
    }
    querySideStory->reset();

    updateSideStoryQuestSceneProgressStatus(sideStoryQuestId, currentQuestSceneId);
}

void UserContext::updateSideStoryQuestSceneProgressStatus(int32_t sideStoryQuestId, int32_t questSceneId) {
    /*
     * Update the progress status.
     */
    auto statusUpdate = db().prepare(R"SQL(
        INSERT INTO i_user_side_story_quest_scene_progress_status (
            user_id,
            current_side_story_quest_id,
            current_side_story_quest_scene_id
        ) VALUES (
            ?,
            ?,
            ?
        ) ON CONFLICT (user_id) DO UPDATE SET
            current_side_story_quest_id = excluded.current_side_story_quest_id,
            current_side_story_quest_scene_id = excluded.current_side_story_quest_scene_id
    )SQL");
    statusUpdate->bind(1, m_userId);
    statusUpdate->bind(2, sideStoryQuestId);
    statusUpdate->bind(3, questSceneId);
    statusUpdate->exec();
}

void UserContext::updateSideStoryQuestSceneProgress(int32_t sideStoryQuestId, int32_t sideStoryQuestSceneId) {
    auto getOrderInfo = db().prepare(R"SQL(
        SELECT
            sort_order
        FROM
            m_side_story_quest_scene
        WHERE
            side_story_quest_id = ? AND
            side_story_quest_scene_id = ?
    )SQL");
    getOrderInfo->bind(1, sideStoryQuestId);
    getOrderInfo->bind(2, sideStoryQuestSceneId);
    if(!getOrderInfo->step()) {
        throw std::runtime_error("no such side story quest, or the requested scene doesn't belong to that quest");
    }

    auto sortOrderOfTheRequestedScene = getOrderInfo->columnInt(0);
    getOrderInfo->reset();

    auto getOverallStatus = db().prepare(
        "SELECT 1 FROM i_user_side_story_quest_scene_progress_status WHERE user_id = ? AND current_side_story_quest_id = ?"
    );
    getOverallStatus->bind(1, m_userId);
    getOverallStatus->bind(2, sideStoryQuestId);
    if(!getOverallStatus->step()) {
        throw std::runtime_error("not currently on this side story quest");
    }
    getOverallStatus->reset();

    auto getSideStoryQuestStatus = db().prepare(R"SQL(
        SELECT
            m_side_story_quest_scene.sort_order
        FROM
            i_user_side_story_quest,
            m_side_story_quest_scene ON
                m_side_story_quest_scene.side_story_quest_id = i_user_side_story_quest.side_story_quest_id AND
                m_side_story_quest_scene.side_story_quest_scene_id = i_user_side_story_quest.head_side_story_quest_scene_id
        WHERE
            user_id = ? AND
            i_user_side_story_quest.side_story_quest_id = ?
    )SQL");
    getSideStoryQuestStatus->bind(1, m_userId);
    getSideStoryQuestStatus->bind(2, sideStoryQuestId);

    if(!getSideStoryQuestStatus->step()) {
        throw std::runtime_error("no record of this side story quest");
    }

    auto currentSortOrder = getSideStoryQuestStatus->columnInt(0);
    getSideStoryQuestStatus->reset();

    m_log.debug("updateSideStoryQuestSceneProgress(quest %d, scene %d): the sort order of the new scene is %d, the current sort order is %d",
                sideStoryQuestId, sideStoryQuestSceneId, sortOrderOfTheRequestedScene, currentSortOrder);

    if(sortOrderOfTheRequestedScene >currentSortOrder) {
        auto updateSideStoryQuestStatus = db().prepare(R"SQL(
            UPDATE i_user_side_story_quest SET head_side_story_quest_scene_id = ?
            WHERE user_id = ? AND side_story_quest_id = ?
        )SQL");
        updateSideStoryQuestStatus->bind(1, sideStoryQuestSceneId);
        updateSideStoryQuestStatus->bind(2, m_userId);
        updateSideStoryQuestStatus->bind(3, sideStoryQuestId);
        updateSideStoryQuestStatus->exec();
    }

    updateSideStoryQuestSceneProgressStatus(sideStoryQuestId, sideStoryQuestSceneId);
}

void UserContext::startExplore(int32_t exploreId, int32_t useConsumableItemId) {
    auto checkNoOngoingExplore = db().prepare("SELECT 1 FROM i_user_explore WHERE user_id = ? AND playing_explore_id != 0");
    checkNoOngoingExplore->bind(1, m_userId);
    if(checkNoOngoingExplore->step()) {
        throw std::runtime_error("an exploration is already ongoing");
    }

    checkNoOngoingExplore->reset();

    auto getItemCount = db().prepare("SELECT consume_item_count FROM m_explore WHERE explore_id = ?");
    getItemCount->bind(1, exploreId);
    if(!getItemCount->step()) {
        throw std::runtime_error("no such exploration");
    }

    auto itemCount = getItemCount->columnInt(0);
    getItemCount->reset();

    if(useConsumableItemId != 0) {
        consumeConsumableItem(useConsumableItemId, itemCount);
    }

    auto recordExplore = db().prepare(R"SQL(
        INSERT INTO i_user_explore (
            user_id,
            is_use_explore_ticket,
            playing_explore_id,
            latest_play_datetime
        ) VALUES (
            ?,
            ?,
            ?,
            current_net_timestamp()
        ) ON CONFLICT (user_id) DO UPDATE SET
            is_use_explore_ticket = excluded.is_use_explore_ticket,
            playing_explore_id = excluded.playing_explore_id,
            latest_play_datetime = excluded.latest_play_datetime
    )SQL");
    recordExplore->bind(1, m_userId);
    recordExplore->bind(2, useConsumableItemId != 0);
    recordExplore->bind(3, exploreId);
    recordExplore->exec();
}

void UserContext::finishExplore(int32_t exploreId, int32_t score, int32_t &assetGradeIconId) {
    retireExplore(exploreId);

    auto getGradeAsset = db().prepare(R"SQL(
        SELECT asset_grade_icon_id
        FROM
            m_explore_grade_score,
            m_explore_grade_asset USING (explore_grade_id)
        WHERE explore_id = ? AND necessary_score <= ?
        ORDER BY necessary_score DESC
        LIMIT 1
    )SQL");
    getGradeAsset->bind(1, exploreId);
    getGradeAsset->bind(2, score);
    if(getGradeAsset->step()) {
        assetGradeIconId = getGradeAsset->columnInt(0);
    } else {
        assetGradeIconId = 0;
    }
    getGradeAsset->reset();

    int32_t currentHighScore = 0;
    auto getHighScore = db().prepare("SELECT max_score FROM i_user_explore_score WHERE user_id = ? AND explore_id = ?");
    getHighScore->bind(1, m_userId);
    getHighScore->bind(2, exploreId);
    if(getHighScore->step()) {
        currentHighScore = getHighScore->columnInt(0);
    }
    getHighScore->reset();


    if(score > currentHighScore) {
        auto storeHighScore = db().prepare(R"SQL(
            INSERT INTO i_user_explore_score (
                user_id,
                explore_id,
                max_score,
                max_score_update_datetime
            ) VALUES (
                ?,
                ?,
                ?,
                current_net_timestamp()
            ) ON CONFLICT (user_id, explore_id) DO UPDATE SET
                max_score = excluded.max_score,
                max_score_update_datetime = excluded.max_score_update_datetime
        )SQL");
        storeHighScore->bind(1, m_userId);
        storeHighScore->bind(2, exploreId);
        storeHighScore->bind(3, score);
        storeHighScore->exec();
    }
}

void UserContext::retireExplore(int32_t exploreId) {
    auto retire = db().prepare("UPDATE i_user_explore SET playing_explore_id = 0 WHERE user_id = ? AND playing_explore_id = ? RETURNING 1");
    retire->bind(1, m_userId);
    retire->bind(2, exploreId);
    if(!retire->step()) {
        throw std::runtime_error("the exploration is not currently ongoing");
    }
}

void UserContext::purgeStaleGifts() {
    auto query = db().prepare(R"SQL(
        DELETE FROM internal_user_gift WHERE user_id = ? AND
            (received_datetime > 0 AND received_datetime + 7 * 24 * 3600 * 1000 < current_net_timestamp()) OR
            (received_datetime = 0 AND expires_datetime != 0 AND expires_datetime < current_net_timestamp())
    )SQL");
    query->bind(1, m_userId);
    query->exec();
}

int32_t UserContext::getNumberOfUnreceivedGifts() {
    auto query = db().prepare(R"SQL(
        SELECT COUNT(*)
        FROM internal_user_gift
        WHERE user_id = ? AND received_datetime = 0 AND (expires_datetime = 0 OR expires_datetime > current_net_timestamp())
    )SQL");
    query->bind(1, m_userId);

    if(query->step()) {
        return query->columnInt(0);
    }

    return 0;
}

bool UserContext::receiveGift(const std::string_view &giftUUID) {
    auto getGiftData = db().prepare(R"SQL(
        UPDATE internal_user_gift
        SET received_datetime = current_net_timestamp()
        WHERE
            user_id = ? AND
            gift_id = ? AND
            received_datetime = 0 AND
            (expires_datetime = 0 OR expires_datetime > current_net_timestamp())
        RETURNING gift_data
    )SQL");
    getGiftData->bind(1, m_userId);
    getGiftData->bind(2, giftUUID);
    if(!getGiftData->step())
        return false;

    auto giftData = getGiftData->columnBlob(0);
    auto giftBytes = getGiftData->columnBytes(0);

    apb::api::gift::GiftCommon gift;
    if(!gift.ParseFromArray(giftData, giftBytes))
        throw std::runtime_error("failed to parse the gift data");

    getGiftData->reset();

    /*
     * TODO: extension (leveling) data
     */

    givePossession(gift.possession_type(), gift.possession_id(), gift.count());

    return true;
}

void UserContext::setPvpDefenseDeck(int32_t deckNumber)  {
    auto query = db().prepare(R"SQL(
        INSERT INTO i_user_pvp_defense_deck (user_id, user_deck_number) VALUES (?, ?)
        ON CONFLICT (user_id) DO UPDATE SET user_deck_number = excluded.user_deck_number
    )SQL");
    query->bind(1, m_userId);
    query->bind(2, deckNumber);
    query->exec();
}

void UserContext::consumeGem(int32_t count, bool paid) {
    if(count < 0)
        throw std::runtime_error("improper number of gems to withdraw");

    if(count == 0)
        return;

    auto queryGem = db().prepare("SELECT paid_gem, free_gem FROM i_user_gem WHERE user_id = ?");
    queryGem->bind(1, m_userId);
    if(!queryGem->step())
        throw std::runtime_error("no gem record for the user");

    auto availableFree = queryGem->columnInt(0);
    auto availablePaid = queryGem->columnInt(1);

    if(!paid && availableFree >= count) {
        auto withdraw = db().prepare("UPDATE i_user_gem SET free_gem = free_gem - ? WHERE user_id = ?");
        withdraw->bind(1, count);
        withdraw->bind(2, m_userId);
        withdraw->exec();
        return;
    }

    if(availablePaid >= count) {
        auto withdraw = db().prepare("UPDATE i_user_gem SET paid_gem = paid_gem - ? WHERE user_id = ?");
        withdraw->bind(1, count);
        withdraw->bind(2, m_userId);
        withdraw->exec();
        return;
    }

    throw std::runtime_error("the user does not have enough gems");
}

void UserContext::buyShopItem(int32_t shopId, int32_t shopItemId) {
    auto getInfo = db().prepare(R"SQL(
        SELECT price_type, price_id, price FROM m_shop_item WHERE shop_item_id = ?
    )SQL");
    getInfo->bind(1, shopItemId);
    if(!getInfo->step()) {
        throw std::runtime_error("no such shop item");
    }

    auto priceType = static_cast<PriceType>(getInfo->columnInt(0));
    auto priceId = getInfo->columnInt(1);
    auto price = getInfo->columnInt(2);

    if(priceType == PriceType::CONSUMABLE_ITEM) {
        consumeConsumableItem(priceId, price);

    } else if(priceType == PriceType::GEM || priceType == PriceType::PAID_GEM) {
        consumeGem(price, priceType == PriceType::PAID_GEM);

    } else if(priceType == PriceType::PLATFORM_PAYMENT) {

        m_log.info("purchasing shop item %d from shop %d with a PLATFORM_PAYMENT price type; platform payment ID: %d",
                   shopId, shopItemId, priceId);

        /*
         * Nothing to be done.
         */
    } else {
        throw std::runtime_error("unsupported PriceType");
    }

    /*
     * TODO: missions
     * TODO: effects (how is that even handled?
     */

    /*
     * NOTE: while m_shop_item_user_level_condition, m_shop_item_additional_content tables
     * exist, they are empty, so we don't handle them.
     */

    auto queryPosessions = db().prepare(R"SQL(
        SELECT
            possession_type,
            possession_id,
            count
        FROM m_shop_item_content_possession
        WHERE shop_item_id = ?
        ORDER BY sort_order
    )SQL");

    /*
     * TODO: overflow.
     */

    queryPosessions->bind(1, shopItemId);
    while(queryPosessions->step()) {
        auto possessionType = queryPosessions->columnInt(0);
        auto possessionId = queryPosessions->columnInt(1);
        auto count = queryPosessions->columnInt(2);

        givePossession(possessionType, possessionId, count);
    }
    queryPosessions->reset();

    auto recordPurchase = db().prepare(R"SQL(
        INSERT INTO i_user_shop_item (
            user_id,
            shop_item_id,
            bought_count,
            latest_bought_count_changed_datetime
        ) VALUES (
            ?, ?,
            1,
            current_net_timestamp()
        )
        ON CONFLICT (user_id, shop_item_id) DO UPDATE SET
            bought_count = bought_count + excluded.bought_count,
            latest_bought_count_changed_datetime = excluded.latest_bought_count_changed_datetime
    )SQL");
    recordPurchase->bind(1, m_userId);
    recordPurchase->bind(2, shopItemId);
    recordPurchase->exec();
}


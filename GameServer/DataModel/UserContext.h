#ifndef USER_CONTEXT_H
#define USER_CONTEXT_H

#include <DataModel/DatabaseContext.h>
#include <DataModel/DatabaseEnums.h>

#include <cstdint>
#include <array>
#include <string>

class Database;

namespace apb::api::deck {
    class Deck;
    class DeckCharacter;
    class DeckCharacterPower;
    class DeckPower;
}

namespace apb::api::quest {
    class QuestReward;
}

namespace google::protobuf {
    template <typename Element>
    class RepeatedPtrField;
}

class JSONWriter;

class UserContext final : public DatabaseContext {
public:
    UserContext(Database &database, int64_t userId);
    UserContext(DatabaseContext &context, int64_t userId);
    ~UserContext() override;

    void buildDefaultDeckIfNoneExists();

    void replaceDeck(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::Deck *deckDefinition);

    void updateDeckName(int32_t deckType, int32_t userDeckNumber, const std::string_view &deckName);

    void refreshDeckPower(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::DeckPower &power);

    void givePossession(int32_t possessionType, int32_t possessionId, int32_t count,
                        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards = nullptr);

    void giveUserExperience(int32_t experience);

    void giveUserDeckExperience(
        int32_t deckType,
        int32_t userDeckNumber,
        int32_t characterExperience,
        int32_t costumeExperience);

    void giveUserCostumeExperience(const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserWeaponExperience(const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserCharacterExperience(int32_t characterId, int32_t characterExperience);

    void serializeTable(const std::string &tableEntityName, JSONWriter &writer);

    void registerDokanConfirmed(int32_t dokanId);

    bool hasWeaponWithId(int32_t weaponId);
    bool hasAnyCompanions();

    void registerNaviCutInPlayed(int32_t cutInId);

    void registerOmikujiDraw(int32_t omikujiId);

    void gameStart();

    void setUserName(const std::string_view &userName);
    void setUserMessage(const std::string_view &userMessage);
    void setUserFavoriteCostume(int32_t costumeId);

    void setUserTutorialProgress(int32_t tutorialType, int32_t progressPhase, int32_t choiceId = 0);

    void updateMainFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId);

    void recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber);
    void startOrRestartMainQuest(int32_t questId, bool isMainFlow, bool isReplayFlow, const std::optional<bool> &isBattleOnly);
    void setMainQuestFlowStatus(QuestFlowType flowType);
    void getOrResetAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber);
    void getAndClearAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber);
    void retireQuest(int32_t questId);
    void finishMainQuest(
        int32_t questId,
        int32_t userDeckNumber,
        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *firstClearRewards,
        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *dropRewards);

    void setMainQuestProgressStatus(int32_t currentQuestSceneId, int32_t headQuestSceneId, QuestFlowType currentQuestFlowType);

    void updateMainQuestProgress();

    void updateMainQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId);

    void updateUserUnlocks();

    bool isQuestCleared(int32_t questId);

    std::optional<int32_t> getWeaponLevel(int32_t weaponId);

    void registerCostumeLevelBonusConfirmed(int32_t costumeId, int32_t newlyConfirmedLevel);

    void queryCostumeRarityAndEnhancementCost(const std::string &costumeUUID, int32_t itemCount,
                                              int32_t &costumeRarity, int32_t &costumeEnhancementCost);

    int32_t consumeEnhancementMaterialAndCalculateTotalEffectValue(int32_t materialId, int32_t materialCount, MaterialType requiredMaterialType,
                                                                   int32_t requiredRarity);

    void consumeConsumableItem(int32_t consumableItemId, int32_t count);

private:

    struct DeckInDatabaseRepresentation {
        std::array<std::string, 3> characterUUIDs;
    };

    enum [[deprecated("needs review, there should be a proper database enum for this or something")]] QuestStateType : int32_t {
        QuestStateType_MainFlowInProgress = 1, // Verified
        QuestStateType_MainFlowComplete = 2 // Verified
    };

    void replaceDeckCharacters(DeckInDatabaseRepresentation &storedDeck,
                               const ::apb::api::deck::Deck *deck);

    void replaceDeckCharacter(std::string &characterUUID,
                              const ::apb::api::deck::DeckCharacter *character);

    void giveUserDeckCharacterExperience(
        const std::string &userDeckCharacterUuid,
        int32_t characterExperience,
        int32_t costumeExperience);

    void updateDeckCharacterPower(const apb::api::deck::DeckCharacterPower &power);

    void issueFirstClearRewardGroup(int64_t firstClearGroupId,
                                    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards = nullptr,
                                    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestDropRewards = nullptr);

    bool isWeaponStoryReleaseConditionSatisfied(
        int32_t weaponId,
        WeaponStoryReleaseConditionType type,
        int32_t value
    );

    void updateWeaponUnlockedStory(int32_t weaponId);

    int64_t m_userId;
};

#endif


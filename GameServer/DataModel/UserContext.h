#ifndef USER_CONTEXT_H
#define USER_CONTEXT_H

#include <DataModel/DatabaseContext.h>
#include <DataModel/DatabaseEnums.h>

#include <LLServices/Logging/LogFacility.h>

#include <cstdint>
#include <array>
#include <string>
#include <unordered_set>

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

namespace apb::api::cageornament {
    class CageOrnamentReward;
}

namespace apb::api::gimmick {
    class GimmickReward;
}

namespace google::protobuf {
    template <typename Element>
    class RepeatedPtrField;

    template <typename K, typename V>
    class Map;
}

namespace LLServices {
    class JSONWriter;
}

class UserContext final : public DatabaseContext {
public:
    struct CharacterBoardDeferredUpdate {
        std::unordered_set<int32_t> charactersToRecalculate;
    };

    UserContext(Database &database, int64_t userId);
    UserContext(DatabaseContext &context, int64_t userId);
    ~UserContext() override;

    void buildDefaultDeckIfNoneExists();

    void replaceDeck(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::Deck *deckDefinition);

    void readDeck(int32_t deckType, int32_t userDeckNumber, apb::api::deck::Deck *deckDefinition);

    void deleteDeck(int32_t deckType, int32_t userDeckNumber);

    void updateDeckName(int32_t deckType, int32_t userDeckNumber, const std::string_view &deckName);

    void refreshDeckPower(int32_t deckType, int32_t userDeckNumber, const apb::api::deck::DeckPower &power);

    void givePossession(int32_t possessionType, int32_t possessionId, int32_t count,
                        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards = nullptr);

    void takePossession(int32_t possessionType, int32_t possessionId, int32_t count);

    void giveUserExperience(int32_t experience);

    void giveUserDeckExperience(
        int32_t deckType,
        int32_t userDeckNumber,
        int32_t characterExperience,
        int32_t costumeExperience);

    void giveUserCostumeExperience(const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserWeaponExperience(const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserCharacterExperience(int32_t characterId, int32_t characterExperience);

    void serializeTable(const std::string &tableEntityName, LLServices::JSONWriter &writer);

    void registerDokanConfirmed(int32_t dokanId);

    [[deprecated("has uncertain semantics around evolution")]] bool hasWeaponWithId(int32_t weaponId);

    bool hasAnyCompanions();

    void registerNaviCutInPlayed(int32_t cutInId);

    void registerOmikujiDraw(int32_t omikujiId);

    void beforeGamePlay();
    void gameStart();

    void setUserName(const std::string_view &userName);
    void setUserMessage(const std::string_view &userMessage);
    void setUserFavoriteCostume(int32_t costumeId);

    void setUserTutorialProgress(int32_t tutorialType, int32_t progressPhase, int32_t choiceId = 0);

    void updateMainFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId);

    void updateReplayFlowSceneProgress(int32_t currentSceneId, int32_t headSceneId);

    void recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber);
    void startMainQuest(int32_t questId, bool isMainFlow, bool isReplayFlow, const std::optional<bool> &isBattleOnly);
    void setMainQuestFlowStatus(QuestFlowType flowType);
    void getOrResetAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber);
    void getAndClearAttributesAtStartOfQuest(int32_t questId, int32_t &userDeckNumber);
    void retireQuest(int32_t questId);
    void finishQuest(
        int32_t questId,
        int32_t userDeckNumber,
        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *firstClearRewards,
        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *dropRewards,
        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *replayFlowFirstClearReward = nullptr);

    void setMainQuestProgressStatus(int32_t currentQuestSceneId, int32_t headQuestSceneId, QuestFlowType currentQuestFlowType);

    void updateMainQuestProgress();

    void updateMainQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId);

    void updateUserUnlocks();

    bool isQuestCleared(int32_t questId);

    std::optional<int32_t> getWeaponLevel(int32_t weaponId);

    void registerCostumeLevelBonusConfirmed(int32_t costumeId, int32_t newlyConfirmedLevel);

    void queryCostumeEnhancementCost(const std::string &costumeUUID, int32_t itemCount,
                                     int32_t &costumeEnhancementCost);

    void queryWeaponEnhancementCost(const std::string &weaponUUID, int32_t itemCount,
                                     int32_t &weaponEnhancementCost);

    int32_t consumeEnhancementMaterialAndCalculateTotalEffectValue(int32_t materialId, int32_t materialCount, MaterialType requiredMaterialType);

    void consumeMaterial(int32_t materialId, int32_t count);
    void consumeConsumableItem(int32_t consumableItemId, int32_t count);

    void setWeaponProtected(const std::string_view &uuid, bool isProtected);

    void enhanceCostumeActiveSkill(const std::string_view &uuid);

    void enhanceWeaponSkill(const std::string_view &uuid, int32_t skillId);

    void enhanceWeaponAbility(const std::string_view &uuid, int32_t Id);

    void updatePortalCageSceneProgress(int32_t sceneId);

    void setMainQuestRoute(int32_t routeId);

    void activateCageOrnament(int32_t cageOrnamentId,
                              google::protobuf::RepeatedPtrField<apb::api::cageornament::CageOrnamentReward> *rewards = nullptr);

    void costumeLimitBreak(const std::string &costumeUUID,
                           const google::protobuf::Map<int32_t, int32_t> &materialsToUse);

    void weaponLimitBreak(const std::string &weaponUUID,
                           const google::protobuf::Map<int32_t, int32_t> &materialsToUse);

    void enhanceCompanion(const std::string &companionUUID);

    void setQuestSceneChoice(
        int32_t questSceneId,
        int32_t choiceNumber,
        int32_t questFlowType);

    void setExtraQuestProgressStatus(int32_t currentQuestId, int32_t currentQuestSceneId, int32_t headQuestSceneId);
    void updateExtraQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId);

    void startExtraQuest(int32_t questId);

    void updateEventQuestSceneProgress(int32_t currentQuestSceneId, int32_t headQuestSceneId);

    void setEventQuestProgressStatus(int32_t eventQuestChapterId, int32_t currentQuestId, int32_t currentQuestSceneId, int32_t headQuestSceneId);

    void startEventQuest(int32_t eventQuestChapterId, int32_t questId, const std::optional<bool> &isBattleOnly);

    void rebirthCharacter(int32_t characterId);

    void releaseCharacterBoardPanel(int32_t panelId, CharacterBoardDeferredUpdate &update);
    void finalizeCharacterBoardUpdate(const CharacterBoardDeferredUpdate &update);

    void initGimmickSequenceSchedule();
    void updateGimmickSequence(int32_t gimmickSequenceScheduleId, int32_t gimmickSequenceId);
    void unlockGimmick(int32_t gimmickSequenceScheduleId, int32_t gimmickSequenceId, int32_t gimmickId);

    void updateGimmickProgress(int32_t gimmickSequenceScheduleId, int32_t gimmickSequenceId, int32_t gimmickId, int32_t gimmickOrnamentIndex,
                               int32_t progressValueBit,
                               google::protobuf::RepeatedPtrField<apb::api::gimmick::GimmickReward> *gimmickOrnamentReward,
                               bool &sequenceCleared,
                               google::protobuf::RepeatedPtrField<apb::api::gimmick::GimmickReward> *gimmickSequenceClearReward);

    void weaponEvolve(const std::string &weaponUUID);

    void weaponAwaken(const std::string &weaponUUID);

    void registerContentsStoryPlayed(int32_t contentsStoryId);

    void costumeAwaken(const std::string &costumeUUID,
                           const google::protobuf::Map<int32_t, int32_t> &materialsToUse);

    void moveSideStoryQuest(int32_t sideStoryQuestId);

    void updateSideStoryQuestSceneProgress(int32_t sideStoryQuestId, int32_t sideStoryQuestSceneId);

private:

    struct CollectedConditionRequirements {
        std::unordered_set<int32_t> missions;
    };

    struct DeckInDatabaseRepresentation {
        std::array<std::string, 3> characterUUIDs;
    };

    /*
     * This is a guess, but appears to be correct.
     */
    enum class QuestStateType : int32_t {
        NotStarted = 0,
        InProgress,
        Cleared = 2
    };

    struct AggregatedBonuses {
        int32_t hp = 0;
        int32_t attack = 0;
        int32_t vitality = 0;
        int32_t agility = 0;
        int32_t criticalRatio = 0;
        int32_t criticalAttack = 0;

        template<typename T>
        void apply(T bonusType, int32_t bonusValue);
    };

    using CharacterBoardReleaseStatus = std::array<uint32_t, 4>;

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

    void issueReplayFlowRewardGroup(int64_t replayFlowGroupId,
                                    google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards = nullptr);

    bool isWeaponStoryReleaseConditionSatisfied(
        int32_t weaponId,
        WeaponStoryReleaseConditionType type,
        int32_t value
    );

    void updateWeaponUnlockedStory(int32_t weaponId);

    void giveUserCostume(
        int32_t costumeId,
        int32_t limitBreakCount = 0,
        int32_t level = 1,
        int32_t activeSkillLevel = 1,
        int32_t awakenCount = 0);
    void giveUserCostumeEnhanced(int32_t costumeEnhancedId);

    void leaveSpecialStates();

    void commonStartQuest(int32_t questId, const std::optional<bool> &isBattleOnly = std::nullopt);

    void readDeckRepresentation(int32_t deckType, int32_t userDeckNumber, DeckInDatabaseRepresentation &deck);
    void readDeckCharacterRepresentation(const std::string_view &deckCharacterUUID, apb::api::deck::DeckCharacter *character);
    void deleteDeckCharacter(const std::string_view &deckCharacterUUID);

    void reevaluateCharacterCostumeLevelBonuses(int32_t characterId);

    CharacterBoardReleaseStatus queryCharacterBoardReleaseStatus(int32_t characterBoardId);
    void storeCharacterBoardReleaseStatus(int32_t characterBoardId, const CharacterBoardReleaseStatus &status);

    static void splitCharacterBoardSortOrder(int32_t sortOrder, uint32_t &word, uint32_t &bit);
    static bool isCharacterBoardPanelReleased(const CharacterBoardReleaseStatus &status, int32_t sortOrder);
    static void setCharacterBoardPanelReleased(CharacterBoardReleaseStatus &status, int32_t sortOrder, bool released);

    static std::string makeFacilityString(int64_t userId);

    bool evaluateCondition(int32_t evaluateConditionId, CollectedConditionRequirements *requirements = nullptr);

    bool evaluateCondition(
        EvaluateConditionFunctionType functionType,
        EvaluateConditionEvaluateType evaluateType,
        const std::vector<int32_t> &functionInputValues,
        CollectedConditionRequirements *requirements = nullptr);

    bool isMissionClear(int32_t missionId);

    void populateWeaponSkillsAndAbilities(const std::string &weaponUUID);
    void populateWeaponNote(const std::string &weaponUUID);

    int32_t getHighestEvolutionOrder(int32_t weaponId);

    void updateSideStoryQuestSceneProgressStatus(int32_t sideStoryQuestId, int32_t questSceneId);

    int64_t m_userId;
    LLServices::LogFacility m_log;
};

#endif


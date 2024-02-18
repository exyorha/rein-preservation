#include "QuestService.h"
#include "DataModel/DatabaseEnums.h"
#include "DataModel/Sqlite/Transaction.h"

#include <DataModel/Sqlite/Statement.h>

QuestService::QuestService(Database &db) : CommonService(db) {

}

QuestService::~QuestService() = default;


::grpc::Status QuestService::UpdateMainFlowSceneProgress(::grpc::ServerContext* context,
                                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    return inChangesetCall("QuestService::UpdateMainFlowSceneProgress", context, request, response, &QuestService::UpdateMainFlowSceneProgressImpl);
}


void QuestService::UpdateMainFlowSceneProgressImpl(int64_t userId,
                                        const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                     ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    // TODO: difference between current_quest_scene_id and head_quest_scene_id?
    auto updateMainFlow = db().prepare(R"SQL(
        UPDATE i_user_main_quest_main_flow_status SET
            current_quest_scene_id = ?,
            head_quest_scene_id = ?
        WHERE user_id = ?
    )SQL");

    updateMainFlow->bind(1, request->quest_scene_id());
    updateMainFlow->bind(2, request->quest_scene_id());
    updateMainFlow->bind(3, userId);
    updateMainFlow->exec();
}

::grpc::Status QuestService::StartMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response) {

    return inChangesetCall("QuestService::StartMainQuest", context, request, response, &QuestService::StartMainQuestImpl);
}

void QuestService::StartMainQuestImpl(int64_t userId, const ::apb::api::quest::StartMainQuestRequest* request,
    ::apb::api::quest::StartMainQuestResponse* response) {

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
    updateStartAttributes->bind(1, userId);
    updateStartAttributes->bind(2, request->quest_id());
    updateStartAttributes->bind(3, request->user_deck_number());
    updateStartAttributes->exec();

    auto getRoute = db().prepare(R"SQL(
        SELECT
            m_main_quest_chapter.main_quest_route_id
        FROM
            m_main_quest_sequence,
            m_main_quest_sequence_group ON m_main_quest_sequence_group.main_quest_sequence_id = m_main_quest_sequence.main_quest_sequence_id,
            m_main_quest_chapter ON m_main_quest_chapter.main_quest_sequence_group_id = m_main_quest_sequence_group.main_quest_sequence_group_id
        WHERE m_main_quest_sequence.quest_id = ?
        LIMIT 1
    )SQL");

    getRoute->bind(1, request->quest_id());
    if(!getRoute->step()) {
        throw std::runtime_error("the quest was not found");
    }

    auto route = getRoute->columnInt64(0);
    getRoute->reset();

    /*
     * TODO: we currently select the first scene in a quest, and this is probably incorrect
     */
    /*
     * TODO: what's the distinction between 'current' scene and 'head' scene? Is it the 'highest reached?'
     */

    auto getFirstScene = db().prepare(R"SQL(
        SELECT
            quest_scene_id
        FROM
            m_quest_scene
        WHERE
            quest_id = ?
        ORDER BY sort_order
        LIMIT 1
    )SQL");

    getFirstScene->bind(1, request->quest_id());
    if(!getFirstScene->step())
        throw std::runtime_error("the quest was not found or has no scenes");

    auto firstScene = getFirstScene->columnInt64(0);


    /*
     * TODO: questFlowType (0 - no active, 1 probably main, replay - ??)
     */
    int32_t questFlowType = QuestFlowType_MainFlow;

    setMainQuestFlowStatus(userId, static_cast<QuestFlowType>(questFlowType));

    setMainQuestProgressStatus(userId, firstScene, firstScene, static_cast<QuestFlowType>(questFlowType));

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
            ?,
            current_net_timestamp()
        )
        ON CONFLICT DO UPDATE SET
            quest_state_type = excluded.quest_state_type,
            is_battle_only = excluded.is_battle_only,
            latest_start_datetime = excluded.latest_start_datetime
    )SQL");


    // TODO: quest_state_type???
    int64_t quest_state_type = 1; // 1 probably in progress, 2 probably completed???

    startQuest->bind(1, userId);
    startQuest->bind(2, request->quest_id());
    startQuest->bind(3, quest_state_type);
    startQuest->bind(4, request->is_battle_only() ? 1 : 0);
    startQuest->exec();

    if(request->is_main_flow()) {

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

        updateMainFlow->bind(1, userId);
        updateMainFlow->bind(2, route);
        updateMainFlow->bind(3, firstScene);
        updateMainFlow->bind(4, firstScene);
        updateMainFlow->exec();
    }

    if(request->is_replay_flow()) {
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

        updateReplayFlow->bind(1, userId);
        updateReplayFlow->bind(2, firstScene);
        updateReplayFlow->bind(3, firstScene);
        updateReplayFlow->exec();
    }

    // TODO: should update i_user_quest_auto_orbit
}


::grpc::Status QuestService::FinishMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::FinishMainQuestRequest* request, ::apb::api::quest::FinishMainQuestResponse* response) {

    return inChangesetCall("QuestService::FinishMainQuest", context, request, response, &QuestService::FinishMainQuestImpl);
}


void QuestService::FinishMainQuestImpl(int64_t userId, const ::apb::api::quest::FinishMainQuestRequest* request,
                                              ::apb::api::quest::FinishMainQuestResponse* response) {

    auto getAttributesAtStart = db().prepare(R"SQL(
        DELETE FROM
            internal_user_quest_last_start_attributes
        WHERE
            user_id = ? AND
            quest_id = ?
        RETURNING
            user_deck_number
    )SQL");
    getAttributesAtStart->bind(1, userId);
    getAttributesAtStart->bind(2, request->quest_id());
    int32_t userDeckNumber = 0;

    while(getAttributesAtStart->step()) {
        userDeckNumber = getAttributesAtStart->columnInt(0);
    }

    /*
     * 'Quit' quest:
     *   - quest_id: <quest ID>
     *   - is retired: true
     *   - story skip type: 1
     *   nothing else is set
     * 'Resume' quest from saved data (when there's some sort of conflict/inconsistency):
     *   - quest_id: <quest ID>
     *   - is retired: true
     *   - story skip type: 1
     */

    if(request->is_retired()) {

        auto updateQuest = db().prepare(R"SQL(
            UPDATE i_user_quest SET
                quest_state_type = ?
            WHERE user_id = ? AND quest_id = ? AND quest_state_type = 1
        )SQL");
        int64_t quest_state_type = 0; // is this always correct???

        updateQuest->bind(1, quest_state_type),
        updateQuest->bind(2, userId);
        updateQuest->bind(3, request->quest_id());
        updateQuest->exec();

    } else {

        if(!request->is_main_flow())
            throw std::runtime_error("only main flow is supported right now");

        if(request->is_annihilated())
            throw std::runtime_error("annihilation flag is not implemented");

        if(request->is_auto_orbit())
            throw std::runtime_error("auto orbit flag is not implemented");

        int64_t quest_state_type = QuestStateType_MainFlowComplete; // is this always correct???

        auto getQuestRewardInfo = db().prepare(R"SQL(
            SELECT
                quest_first_clear_reward_group_id,
                quest_pickup_reward_group_id,
                gold,
                user_exp,
                character_exp,
                costume_exp
            FROM
                m_quest
            WHERE quest_id = ?
        )SQL");

        int32_t questFirstClearRewardGroupId = 0;
        int32_t questPickupRewardGroupId = 0;
        int32_t gold = 0;
        int32_t userExperience = 0;
        int32_t characterExperience = 0;
        int32_t costumeExperience = 0;

        getQuestRewardInfo->bind(1, request->quest_id());
        if(getQuestRewardInfo->step()) {
            questFirstClearRewardGroupId = getQuestRewardInfo->columnInt(0);
            questPickupRewardGroupId = getQuestRewardInfo->columnInt(1);
            gold = getQuestRewardInfo->columnInt(2);
            userExperience = getQuestRewardInfo->columnInt(3);
            characterExperience = getQuestRewardInfo->columnInt(4);
            costumeExperience = getQuestRewardInfo->columnInt(5);
        }

        auto updateQuest = db().prepare(R"SQL(
            UPDATE i_user_quest SET
                quest_state_type = ?,
                clear_count = clear_count + 1,
                last_clear_datetime = current_net_timestamp()
            WHERE user_id = ? AND quest_id = ? AND quest_state_type = 1
            RETURNING clear_count
        )SQL");

        updateQuest->bind(1, QuestStateType_MainFlowComplete),
        updateQuest->bind(2, userId);
        updateQuest->bind(3, request->quest_id());
        while(updateQuest->step()) {
            auto clearCount = updateQuest->columnInt(0);
            if(clearCount == 1 && questFirstClearRewardGroupId != 0) {
               issueFirstClearRewardGroup(userId, questFirstClearRewardGroupId,
                                          response->mutable_first_clear_reward(),
                                          response->mutable_drop_reward());
            }
        }


        if(gold != 0) {
            givePossession(userId,
                           static_cast<int32_t>(PossessionType::CONSUMABLE_ITEM),
                           consumableItemIdForGold(),
                           gold
            );
        }

        if(userExperience != 0) {
            giveUserExperience(userId, userExperience);
        }

        if(userDeckNumber != 0) {
            giveUserDeckExperience(userId,
                                   static_cast<int32_t>(DeckType::QUEST),
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

            givePossession(userId, type, id, count, response->mutable_drop_reward());
        }

        setMainQuestFlowStatus(userId, QuestFlowType_MainFlow);
    }

    setMainQuestProgressStatus(userId, 0, 0, QuestFlowType_NoFlow);

    // TODO: IUserMainQuestMainFlowStatus should be set to the 'next scene', possibly in the next quest

    updateMainQuestProgress(userId);
}

void QuestService::updateMainQuestProgress(int64_t userId) {
    /*
     * Get the selected route, if any.
     */

    int64_t route = 0;

    auto getCurrentRoute = db().prepare("SELECT current_main_quest_route_id FROM i_user_main_quest_main_flow_status WHERE user_id = ?");
    getCurrentRoute->bind(1, userId);
    if(getCurrentRoute->step())
        route = getCurrentRoute->columnInt64(0);

    printf("Current route: %ld\n", route);

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
                m_main_quest_season ON m_main_quest_season.main_quest_season_id = m_main_quest_route.main_quest_season_id,
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
        checkIfThisQuestComplete->bind(1, userId);

        while(allQuestsInOrder->step()) {
            auto questId = allQuestsInOrder->columnInt64(0);

            checkIfThisQuestComplete->bind(2, questId);
            int32_t stateType = 0;

            if(checkIfThisQuestComplete->step()) {
                stateType = checkIfThisQuestComplete->columnInt(0);
            }
            checkIfThisQuestComplete->reset();

            printf("route quest %ld has state_type %d\n", questId, stateType);

            if(stateType < 2) {
                /*
                 * The quest is not complete.
                 */
                foundIncompleteQuest = questId;
                break;
            }
        }

        allQuestsInOrder->reset();

        if(foundIncompleteQuest != 0) {
            printf("setting the main quest flow to start from %ld\n", foundIncompleteQuest);

            int64_t firstScene = 0 ;
            auto getFirstScene = db().prepare("SELECT quest_scene_id FROM m_quest_scene WHERE quest_id = ? ORDER BY sort_order LIMIT 1");
            getFirstScene->bind(1, foundIncompleteQuest);
            if(getFirstScene->step())
                firstScene = getFirstScene->columnInt64(0);

            printf("first scene: %ld\n", firstScene);

            if(firstScene != 0) {

                /*
                 * TODO: 'last quest scene' is probably 'end of route!'
                 */

                auto updateNextScene = db().prepare("UPDATE i_user_main_quest_main_flow_status SET current_quest_scene_id = ?, head_quest_scene_id = ? WHERE user_id = ?");
                updateNextScene->bind(1, firstScene);
                updateNextScene->bind(2, firstScene);
                updateNextScene->bind(3, userId);
                updateNextScene->exec();
            }
        }
    }

}

::grpc::Status QuestService::UpdateMainQuestSceneProgress(::grpc::ServerContext* context,
    const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {

    return inChangesetCall("QuestService::UpdateMainQuestSceneProgress", context, request, response, &QuestService::UpdateMainQuestSceneProgressImpl);
}

void QuestService::UpdateMainQuestSceneProgressImpl(int64_t userId,
    const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {

    auto update = db().prepare("UPDATE i_user_main_quest_progress_status SET current_quest_scene_id = ?, head_quest_scene_id = ? WHERE user_id = ?");

    /*
     * TODO: what's the difference between the 'current' and 'head' scene ID?
     */

    update->bind(1, request->quest_scene_id());
    update->bind(2, request->quest_scene_id());
    update->bind(3, userId);
    update->exec();
}

void QuestService::setMainQuestProgressStatus(int64_t userId,
                                              int32_t currentQuestSceneId, int32_t headQuestSceneId, QuestFlowType currentQuestFlowType) {
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
    updateMainQuestProgressStatus->bind(1, userId);
    updateMainQuestProgressStatus->bind(2, currentQuestSceneId);
    updateMainQuestProgressStatus->bind(3, headQuestSceneId);
    updateMainQuestProgressStatus->bind(4, currentQuestFlowType);
    updateMainQuestProgressStatus->exec();
}

void QuestService::setMainQuestFlowStatus(int64_t userId, QuestFlowType flowType) {

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

    updateMainQuestFlowStatus->bind(1, userId);
    updateMainQuestFlowStatus->bind(2, flowType);
    updateMainQuestFlowStatus->exec();
}

void QuestService::issueAllFirstClearRewards() {
    sqlite::Transaction transaction(&db());

    auto getQuests = db().prepare(R"SQL(
        SELECT
            i_user_quest.user_id,
            m_quest.quest_first_clear_reward_group_id
        FROM
            i_user_quest,
            m_quest ON m_quest.quest_id = i_user_quest.quest_id
        WHERE
            clear_count > 0 AND
            m_quest.quest_first_clear_reward_group_id != 0
    )SQL");

    while(getQuests->step()) {
        auto userId = getQuests->columnInt64(0);
        auto firstClearGroupId = getQuests->columnInt64(1);

        issueFirstClearRewardGroup(userId, firstClearGroupId);
    }

    transaction.commit();
}

void QuestService::issueFirstClearRewardGroup(int64_t userId, int64_t firstClearGroupId,
                                              google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards,
                                              google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestDropRewards) {

    printf("QuestService: issuing first clear reward group %ld to user %ld\n", firstClearGroupId, userId);

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

        printf("QuestService: awarding first clear reward to %ld: reward type %d, possession type %d, possession id %d, count %d, is pickup? %d\n",
               userId, rewardType, possessionType, possessionId, count, isPickup);

        auto list = addToQuestRewards;
        if(isPickup) {
            list = addToQuestDropRewards;
        }

        givePossession(userId, possessionType, possessionId, count, list);
    }
}

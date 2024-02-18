#include "QuestService.h"
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

#if 0
    response->mutable_diff_user_data()->emplace("IUserMainQuestMainFlowStatus", stream.str());
#endif
#if 0

    response->mutable_diff_user_data()->emplace(
        "IUserMainQuestFlowStatus",
        R"json([{"userId":1,"currentQuestFlowType":1,"latestVersion":1}])json"
    );
#endif

#if 0
    auto &quest = (*response->mutable_diff_user_data())["IUserQuest"];

    quest.set_update_records_json(
        R"json([{"userId":1,"questId":1,"questStateType":1,"isBattleOnly":false,"latestStartDatetime":1706703062000,"clearCount":0,"dailyClearCount":0,"lastClearDatetime":28800000,"shortestClearFrames":2147483647,"latestVersion":1}])json"
    );

    quest.set_delete_keys_json("[]");

    auto &mainQuestFlow = (*response->mutable_diff_user_data())["IUserMainQuestFlowStatus"];
    mainQuestFlow.set_update_records_json(
        R"json([{"userId":1,"currentQuestFlowType":1,"latestVersion":1}])json"
    );
    mainQuestFlow.set_delete_keys_json("[]");
#if 0
    auto &mainQuestMainFlow = (*response->mutable_diff_user_data())["IUserMainQuestMainFlowStatus"];
    mainQuestMainFlow.set_update_records_json(
        R"json([{"userId":1,"currentMainQuestRouteId":1,"currentQuestSceneId":2,"headQuestSceneId":2,"isReachedLastQuestScene":false,"latestVersion":1}])json"
    );
    mainQuestMainFlow.set_delete_keys_json("[]");
#endif


    auto &mainQuestProgress = (*response->mutable_diff_user_data())["IUserMainQuestProgressStatus"];

    mainQuestProgress.set_update_records_json(
        R"json([{"userId":1,"currentQuestSceneId":2,"headQuestSceneId":2,"currentQuestFlowType":1,"latestVersion":1}])json"
    );
    mainQuestProgress.set_delete_keys_json("[]");

    auto &questAutoOrbit = (*response->mutable_diff_user_data())["IUserQuestAutoOrbit"];
    questAutoOrbit.set_update_records_json(
        R"json([{"userId":1,"questType":1,"chapterId":1,"questId":1,"maxAutoOrbitCount":0,"clearedAutoOrbitCount":0,"lastClearDatetime":28800000,"latestVersion":1}])json"
    );
    questAutoOrbit.set_delete_keys_json("[]");
#endif

//    return grpc::Status::OK;
}


::grpc::Status QuestService::FinishMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::FinishMainQuestRequest* request, ::apb::api::quest::FinishMainQuestResponse* response) {

    return inChangesetCall("QuestService::FinishMainQuest", context, request, response, &QuestService::FinishMainQuestImpl);
}


void QuestService::FinishMainQuestImpl(int64_t userId, const ::apb::api::quest::FinishMainQuestRequest* request,
                                              ::apb::api::quest::FinishMainQuestResponse* response) {

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

        auto updateQuest = db().prepare(R"SQL(
            UPDATE i_user_quest SET
                quest_state_type = ?,
                clear_count = clear_count + 1,
                last_clear_datetime = current_net_timestamp()
            WHERE user_id = ? AND quest_id = ? AND quest_state_type = 1
            RETURNING clear_count
        )SQL");

        // TODO: daily clear management

        updateQuest->bind(1, QuestStateType_MainFlowComplete),
        updateQuest->bind(2, userId);
        updateQuest->bind(3, request->quest_id());
        while(updateQuest->step()) {
            auto clearCount = updateQuest->columnInt(0);
            if(clearCount == 1) {
                auto getQuestFirstClearRewardGroup = db().prepare("SELECT quest_first_clear_reward_group_id FROM m_quest WHERE quest_id = ?");
                getQuestFirstClearRewardGroup->bind(1, request->quest_id());
                while(getQuestFirstClearRewardGroup->step()) {
                    auto rewardGroup = getQuestFirstClearRewardGroup->columnInt(0);
                    if(rewardGroup != 0) {
                        issueFirstClearRewardGroup(userId, rewardGroup);
                    }
                }
            }
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

void QuestService::issueFirstClearRewardGroup(int64_t userId, int64_t firstClearGroupId) {
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

        givePossession(userId, possessionType, possessionId, count);
    }
}

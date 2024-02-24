#include "QuestService.h"
#include "DataModel/DatabaseEnums.h"
#include "DataModel/Sqlite/Transaction.h"

#include <DataModel/Sqlite/Statement.h>
#include <stdexcept>

QuestService::QuestService(Database &db) : CommonService(db) {

}

QuestService::~QuestService() = default;


::grpc::Status QuestService::UpdateMainFlowSceneProgress(::grpc::ServerContext* context,
                                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    return inChangesetCall("QuestService::UpdateMainFlowSceneProgress", context, request, response, &QuestService::UpdateMainFlowSceneProgressImpl);
}


void QuestService::UpdateMainFlowSceneProgressImpl(UserContext &user,
                                        const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                     ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    // TODO: difference between current_quest_scene_id and head_quest_scene_id?
    user.updateMainFlowSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

::grpc::Status QuestService::StartMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response) {

    return inChangesetCall("QuestService::StartMainQuest", context, request, response, &QuestService::StartMainQuestImpl);
}

void QuestService::StartMainQuestImpl(UserContext &user, const ::apb::api::quest::StartMainQuestRequest* request,
    ::apb::api::quest::StartMainQuestResponse* response) {

    user.recordQuestStartAttributes(request->quest_id(), request->user_deck_number());
    user.startOrRestartMainQuest(request->quest_id(), request->is_main_flow(), request->is_replay_flow(), request->is_battle_only());
}

::grpc::Status QuestService::RestartMainQuest(::grpc::ServerContext* context,
                                    const ::apb::api::quest::RestartMainQuestRequest* request, ::apb::api::quest::RestartMainQuestResponse* response) {

    return inChangesetCall("QuestService::RestartMainQuest", context, request, response, &QuestService::RestartMainQuestImpl);
}

void QuestService::RestartMainQuestImpl(UserContext &user,
                        const ::apb::api::quest::RestartMainQuestRequest* request,
                        ::apb::api::quest::RestartMainQuestResponse* response) {

    int32_t userDeckNumber;
    user.getOrResetAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);

    /*
     * TODO: is it correct to always set is_replay_flow = false here?
     */
    user.startOrRestartMainQuest(request->quest_id(), request->is_main_flow(), false, std::nullopt);

}

::grpc::Status QuestService::FinishMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::FinishMainQuestRequest* request, ::apb::api::quest::FinishMainQuestResponse* response) {

    return inChangesetCall("QuestService::FinishMainQuest", context, request, response, &QuestService::FinishMainQuestImpl);
}


void QuestService::FinishMainQuestImpl(UserContext &user, const ::apb::api::quest::FinishMainQuestRequest* request,
                                              ::apb::api::quest::FinishMainQuestResponse* response) {

    int32_t userDeckNumber;
    user.getAndClearAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);


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
        user.retireQuest(request->quest_id());

    } else {


        if(!request->is_main_flow())
            throw std::runtime_error("only main flow is supported right now");

        if(request->is_annihilated())
            throw std::runtime_error("annihilation flag is not implemented");

        if(request->is_auto_orbit())
            throw std::runtime_error("auto orbit flag is not implemented");

        user.finishMainQuest(request->quest_id(),
                             userDeckNumber,
                             response->mutable_first_clear_reward(),
                             response->mutable_drop_reward());

    }

    user.setMainQuestProgressStatus(0, 0, QuestFlowType::UNKNOWN);

    // TODO: IUserMainQuestMainFlowStatus should be set to the 'next scene', possibly in the next quest

    user.updateMainQuestProgress();
}

::grpc::Status QuestService::UpdateMainQuestSceneProgress(::grpc::ServerContext* context,
    const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {

    return inChangesetCall("QuestService::UpdateMainQuestSceneProgress", context, request, response, &QuestService::UpdateMainQuestSceneProgressImpl);
}

void QuestService::UpdateMainQuestSceneProgressImpl(UserContext &user,
    const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {

    /*
     * TODO: what's the difference between the 'current' and 'head' scene ID?
     */

    user.updateMainQuestSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

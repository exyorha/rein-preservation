#include "QuestService.h"
#include "DataModel/DatabaseEnums.h"
#include "DataModel/Sqlite/Transaction.h"

#include <DataModel/Sqlite/Statement.h>
#include <stdexcept>

QuestService::QuestService(Database &db) : CommonService(db) {

}

QuestService::~QuestService() = default;


void QuestService::UpdateMainFlowSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::UpdateMainFlowSceneProgress", controller, request, response, done, &QuestService::UpdateMainFlowSceneProgressImpl);
}


void QuestService::UpdateMainFlowSceneProgressImpl(UserContext &user,
                                        const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                     ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    // TODO: difference between current_quest_scene_id and head_quest_scene_id?
    user.updateMainFlowSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

void QuestService::UpdateMainQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::UpdateMainQuestSceneProgress", controller, request, response, done, &QuestService::UpdateMainQuestSceneProgressImpl);
}

void QuestService::UpdateMainQuestSceneProgressImpl(UserContext &user,
    const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {

    /*
     * TODO: what's the difference between the 'current' and 'head' scene ID?
     */

    user.updateMainQuestSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

void QuestService::UpdateExtraQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateExtraQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateExtraQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::UpdateExtraQuestSceneProgress", controller, request, response, done, &QuestService::UpdateExtraQuestSceneProgressImpl);
}

void QuestService::UpdateExtraQuestSceneProgressImpl(UserContext &user,
    const ::apb::api::quest::UpdateExtraQuestSceneProgressRequest* request, ::apb::api::quest::UpdateExtraQuestSceneProgressResponse* response) {

    /*
     * TODO: what's the difference between the 'current' and 'head' scene ID?
     */

    user.updateExtraQuestSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

void QuestService::UpdateEventQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateEventQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateEventQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::UpdateEventQuestSceneProgress", controller, request, response, done, &QuestService::UpdateEventQuestSceneProgressImpl);
}

void QuestService::UpdateEventQuestSceneProgressImpl(UserContext &user,
    const ::apb::api::quest::UpdateEventQuestSceneProgressRequest* request, ::apb::api::quest::UpdateEventQuestSceneProgressResponse* response) {

    /*
     * TODO: what's the difference between the 'current' and 'head' scene ID?
     */

    user.updateEventQuestSceneProgress(request->quest_scene_id(), request->quest_scene_id());
}

void QuestService::StartMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartMainQuestRequest* request,
                        ::apb::api::quest::StartMainQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::StartMainQuest", controller, request, response, done, &QuestService::StartMainQuestImpl);
}

void QuestService::StartMainQuestImpl(UserContext &user, const ::apb::api::quest::StartMainQuestRequest* request,
    ::apb::api::quest::StartMainQuestResponse* response) {

    user.recordQuestStartAttributes(request->quest_id(), request->user_deck_number());
    user.startMainQuest(request->quest_id(), request->is_main_flow(), request->is_replay_flow(), request->is_battle_only());
}

void QuestService::RestartMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartMainQuestRequest* request,
                        ::apb::api::quest::RestartMainQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::RestartMainQuest", controller, request, response, done, &QuestService::RestartMainQuestImpl);
}

void QuestService::RestartMainQuestImpl(UserContext &user,
                        const ::apb::api::quest::RestartMainQuestRequest* request,
                        ::apb::api::quest::RestartMainQuestResponse* response) {

    int32_t userDeckNumber;
    user.getOrResetAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);
    response->set_deck_number(userDeckNumber);
}

void QuestService::FinishMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishMainQuestRequest* request,
                        ::apb::api::quest::FinishMainQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::FinishMainQuest", controller, request, response, done, &QuestService::FinishMainQuestImpl);
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

        user.finishQuest(request->quest_id(),
                         userDeckNumber,
                         response->mutable_first_clear_reward(),
                         response->mutable_drop_reward());

        user.setMainQuestFlowStatus(QuestFlowType::MAIN_FLOW);
        user.updateUserUnlocks();
    }

    user.setMainQuestProgressStatus(0, 0, QuestFlowType::UNKNOWN);

    // TODO: IUserMainQuestMainFlowStatus should be set to the 'next scene', possibly in the next quest

    user.updateMainQuestProgress();
}


void QuestService::StartExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartExtraQuestRequest* request,
                        ::apb::api::quest::StartExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::StartExtraQuest", controller, request, response, done, &QuestService::StartExtraQuestImpl);
}

void QuestService::StartExtraQuestImpl(UserContext &user, const ::apb::api::quest::StartExtraQuestRequest* request,
    ::apb::api::quest::StartExtraQuestResponse* response) {

    user.recordQuestStartAttributes(request->quest_id(), request->user_deck_number());
    user.startExtraQuest(request->quest_id());
}

void QuestService::RestartExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartExtraQuestRequest* request,
                        ::apb::api::quest::RestartExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::RestartExtraQuest", controller, request, response, done, &QuestService::RestartExtraQuestImpl);
}

void QuestService::RestartExtraQuestImpl(UserContext &user,
                        const ::apb::api::quest::RestartExtraQuestRequest* request,
                        ::apb::api::quest::RestartExtraQuestResponse* response) {

    int32_t userDeckNumber;
    user.getOrResetAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);
    response->set_deck_number(userDeckNumber);
}

void QuestService::FinishExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishExtraQuestRequest* request,
                        ::apb::api::quest::FinishExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::FinishExtraQuest", controller, request, response, done, &QuestService::FinishExtraQuestImpl);
}


void QuestService::FinishExtraQuestImpl(UserContext &user, const ::apb::api::quest::FinishExtraQuestRequest* request,
                                              ::apb::api::quest::FinishExtraQuestResponse* response) {

    int32_t userDeckNumber;
    user.getAndClearAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);

    if(request->is_retired()) {
        user.retireQuest(request->quest_id());

    } else {

        if(request->is_annihilated())
            throw std::runtime_error("annihilation flag is not implemented");

        user.finishQuest(request->quest_id(),
                             userDeckNumber,
                             response->mutable_first_clear_reward(),
                             response->mutable_drop_reward());

        user.updateUserUnlocks();

    }

    user.setExtraQuestProgressStatus(0, 0, 0);
}

void QuestService::StartEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartEventQuestRequest* request,
                        ::apb::api::quest::StartEventQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::StartEventQuest", controller, request, response, done, &QuestService::StartEventQuestImpl);
}

void QuestService::StartEventQuestImpl(UserContext &user, const ::apb::api::quest::StartEventQuestRequest* request,
    ::apb::api::quest::StartEventQuestResponse* response) {

    // TODO: auto orbit???

    user.recordQuestStartAttributes(request->quest_id(), request->user_deck_number());
    user.startEventQuest(request->event_quest_chapter_id(), request->quest_id(), request->is_battle_only());
}

void QuestService::RestartEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartEventQuestRequest* request,
                        ::apb::api::quest::RestartEventQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::RestartEventQuest", controller, request, response, done, &QuestService::RestartEventQuestImpl);
}

void QuestService::RestartEventQuestImpl(UserContext &user,
                        const ::apb::api::quest::RestartEventQuestRequest* request,
                        ::apb::api::quest::RestartEventQuestResponse* response) {

    int32_t userDeckNumber;
    user.getOrResetAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);
    response->set_deck_number(userDeckNumber);
}

void QuestService::FinishEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishEventQuestRequest* request,
                        ::apb::api::quest::FinishEventQuestResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::FinishEventQuest", controller, request, response, done, &QuestService::FinishEventQuestImpl);
}


void QuestService::FinishEventQuestImpl(UserContext &user, const ::apb::api::quest::FinishEventQuestRequest* request,
                                              ::apb::api::quest::FinishEventQuestResponse* response) {

    int32_t userDeckNumber;
    user.getAndClearAttributesAtStartOfQuest(request->quest_id(), userDeckNumber);

    if(request->is_retired()) {
        user.retireQuest(request->quest_id());

    } else {

        if(request->is_annihilated())
            throw std::runtime_error("annihilation flag is not implemented");

        user.finishQuest(request->quest_id(),
                             userDeckNumber,
                             response->mutable_first_clear_reward(),
                             response->mutable_drop_reward());

        user.updateUserUnlocks();

    }

    user.setEventQuestProgressStatus(0, 0, 0, 0);
}

void QuestService::SetRoute(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::SetRouteRequest* request,
                        ::apb::api::quest::SetRouteResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::SetRoute", controller, request, response, done, &QuestService::SetRouteImpl);
}

void QuestService::SetRouteImpl(
    UserContext &user,
    const ::apb::api::quest::SetRouteRequest* request,
    ::apb::api::quest::SetRouteResponse* response) {

    user.setMainQuestRoute(request->main_quest_route_id());
}

void QuestService::SetQuestSceneChoice(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::SetQuestSceneChoiceRequest* request,
                        ::apb::api::quest::SetQuestSceneChoiceResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("QuestService::SetQuestSceneChoice", controller, request, response, done, &QuestService::SetQuestSceneChoiceImpl);
}

void QuestService::SetQuestSceneChoiceImpl(
    UserContext &user,
    const ::apb::api::quest::SetQuestSceneChoiceRequest* request,
    ::apb::api::quest::SetQuestSceneChoiceResponse* response) {

    user.setQuestSceneChoice(
        request->quest_scene_id(),
        request->choice_number(),
        request->quest_flow_type());
}

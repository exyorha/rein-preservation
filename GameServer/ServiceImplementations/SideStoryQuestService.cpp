#include <ServiceImplementations/SideStoryQuestService.h>

SideStoryQuestService::SideStoryQuestService(Database &db) : CommonService(db) {

}

SideStoryQuestService::~SideStoryQuestService() = default;

void SideStoryQuestService::MoveSideStoryQuestProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::sidestoryquest::MoveSideStoryQuestRequest* request,
                            ::apb::api::sidestoryquest::MoveSideStoryQuestResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("SideStoryQuestService::MoveSideStoryQuestProgress", controller, request, response, done,
                           &SideStoryQuestService::MoveSideStoryQuestProgressImpl);
}

void SideStoryQuestService::UpdateSideStoryQuestSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressRequest* request,
                            ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("SideStoryQuestService::UpdateSideStoryQuestSceneProgress", controller, request, response, done,
                           &SideStoryQuestService::UpdateSideStoryQuestSceneProgressImpl);
}

void SideStoryQuestService::MoveSideStoryQuestProgressImpl(UserContext &user,
                                    const ::apb::api::sidestoryquest::MoveSideStoryQuestRequest* request,
                                    ::apb::api::sidestoryquest::MoveSideStoryQuestResponse* response) {

    user.moveSideStoryQuest(request->side_story_quest_id());

}

void SideStoryQuestService::UpdateSideStoryQuestSceneProgressImpl(UserContext &user,
                                            const ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressRequest* request,
                                            ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressResponse* response) {

    user.updateSideStoryQuestSceneProgress(request->side_story_quest_id(), request->side_story_quest_scene_id());
}

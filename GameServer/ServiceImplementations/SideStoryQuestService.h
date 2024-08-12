#ifndef SERVICE_IMPLEMENTATIONS_SIDE_STORY_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_SIDE_STORY_QUEST_SERVICE_H

#include <service/SideStoryQuestService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is complete.
 */
class SideStoryQuestService final : public apb::api::sidestoryquest::SideStoryQuestService, public CommonService {
public:
    SideStoryQuestService(Database &db, const GameServerConfiguration &config);
    ~SideStoryQuestService();

    SideStoryQuestService(const SideStoryQuestService &other) = delete;
    SideStoryQuestService &operator =(const SideStoryQuestService &other) = delete;

    void MoveSideStoryQuestProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::sidestoryquest::MoveSideStoryQuestRequest* request,
                        ::apb::api::sidestoryquest::MoveSideStoryQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateSideStoryQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressRequest* request,
                        ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void MoveSideStoryQuestProgressImpl(UserContext &user,
                                        const ::apb::api::sidestoryquest::MoveSideStoryQuestRequest* request,
                                        ::apb::api::sidestoryquest::MoveSideStoryQuestResponse* response);

    void UpdateSideStoryQuestSceneProgressImpl(UserContext &user,
                                               const ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressRequest* request,
                                               ::apb::api::sidestoryquest::UpdateSideStoryQuestSceneProgressResponse* response);
};

#endif

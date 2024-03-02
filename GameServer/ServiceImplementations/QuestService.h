#ifndef SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H

#include <service/QuestService.pb.h>

#include <ServiceImplementations/CommonService.h>

class QuestService final : public apb::api::quest::QuestService, public CommonService {
public:
    explicit QuestService(Database &db);
    ~QuestService();

    QuestService(const QuestService &other) = delete;
    QuestService &operator =(const QuestService &other) = delete;

    void UpdateMainFlowSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response,
                            ::google::protobuf::Closure* done) override;

    void UpdateMainQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void StartMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartMainQuestRequest* request,
                        ::apb::api::quest::StartMainQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RestartMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartMainQuestRequest* request,
                        ::apb::api::quest::RestartMainQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishMainQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishMainQuestRequest* request,
                        ::apb::api::quest::FinishMainQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetRoute(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::SetRouteRequest* request,
                        ::apb::api::quest::SetRouteResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetQuestSceneChoice(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::SetQuestSceneChoiceRequest* request,
                        ::apb::api::quest::SetQuestSceneChoiceResponse* response,
                        ::google::protobuf::Closure* done) override;

private:

    void UpdateMainFlowSceneProgressImpl(UserContext &user,
                                         const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                         ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response);

    void StartMainQuestImpl(UserContext &user, const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response);

    void RestartMainQuestImpl(UserContext &user,
                          const ::apb::api::quest::RestartMainQuestRequest* request,
                          ::apb::api::quest::RestartMainQuestResponse* response);

    void FinishMainQuestImpl(UserContext &user, const ::apb::api::quest::FinishMainQuestRequest* request,
                             ::apb::api::quest::FinishMainQuestResponse* response);

    void UpdateMainQuestSceneProgressImpl(UserContext &user,
                                          const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                                          ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response);

    void SetRouteImpl(
        UserContext &user,
        const ::apb::api::quest::SetRouteRequest* request,
        ::apb::api::quest::SetRouteResponse* response);

    void SetQuestSceneChoiceImpl(UserContext &user,
                        const ::apb::api::quest::SetQuestSceneChoiceRequest* request,
                        ::apb::api::quest::SetQuestSceneChoiceResponse* response);

    void recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber);
};

#endif


#ifndef SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H

#include <service/QuestService.pb.h>

#include <ServiceImplementations/CommonService.h>

class QuestService final : public apb::api::quest::QuestService, public CommonService {
public:
    QuestService(Database &db, const GameServerConfiguration &config);
    ~QuestService();

    QuestService(const QuestService &other) = delete;
    QuestService &operator =(const QuestService &other) = delete;

    void UpdateMainFlowSceneProgress(::google::protobuf::RpcController* controller,
                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response,
                            ::google::protobuf::Closure* done) override;

    void UpdateReplayFlowSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateReplayFlowSceneProgressRequest* request,
                        ::apb::api::quest::UpdateReplayFlowSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateMainQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateExtraQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateExtraQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateExtraQuestSceneProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateEventQuestSceneProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::UpdateEventQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateEventQuestSceneProgressResponse* response,
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

    void StartExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartExtraQuestRequest* request,
                        ::apb::api::quest::StartExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RestartExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartExtraQuestRequest* request,
                        ::apb::api::quest::RestartExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishExtraQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishExtraQuestRequest* request,
                        ::apb::api::quest::FinishExtraQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void StartEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::StartEventQuestRequest* request,
                        ::apb::api::quest::StartEventQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RestartEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::RestartEventQuestRequest* request,
                        ::apb::api::quest::RestartEventQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishEventQuest(::google::protobuf::RpcController* controller,
                        const ::apb::api::quest::FinishEventQuestRequest* request,
                        ::apb::api::quest::FinishEventQuestResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishAutoOrbit(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::quest::FinishAutoOrbitResponse* response,
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

    void UpdateReplayFlowSceneProgressImpl(UserContext &user,
                                           const ::apb::api::quest::UpdateReplayFlowSceneProgressRequest* request,
                                           ::apb::api::quest::UpdateReplayFlowSceneProgressResponse* response);

    void UpdateMainQuestSceneProgressImpl(UserContext &user,
                                          const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                                          ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response);

    void UpdateExtraQuestSceneProgressImpl(UserContext &user,
                                           const ::apb::api::quest::UpdateExtraQuestSceneProgressRequest* request,
                                           ::apb::api::quest::UpdateExtraQuestSceneProgressResponse* response);

    void UpdateEventQuestSceneProgressImpl(UserContext &user,
                                           const ::apb::api::quest::UpdateEventQuestSceneProgressRequest* request,
                        ::apb::api::quest::UpdateEventQuestSceneProgressResponse* response);

    void StartMainQuestImpl(UserContext &user, const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response);

    void RestartMainQuestImpl(UserContext &user,
                          const ::apb::api::quest::RestartMainQuestRequest* request,
                          ::apb::api::quest::RestartMainQuestResponse* response);

    void FinishMainQuestImpl(UserContext &user, const ::apb::api::quest::FinishMainQuestRequest* request,
               ::apb::api::quest::FinishMainQuestResponse* response);

    void StartExtraQuestImpl(UserContext &user,const ::apb::api::quest::StartExtraQuestRequest* request,
                        ::apb::api::quest::StartExtraQuestResponse* response);

    void RestartExtraQuestImpl(UserContext &user,
                            const ::apb::api::quest::RestartExtraQuestRequest* request,
                        ::apb::api::quest::RestartExtraQuestResponse* response);

    void FinishExtraQuestImpl(UserContext &user,
                            const ::apb::api::quest::FinishExtraQuestRequest* request,
                        ::apb::api::quest::FinishExtraQuestResponse* response);

    void StartEventQuestImpl(UserContext &user,
                            const ::apb::api::quest::StartEventQuestRequest* request,
                        ::apb::api::quest::StartEventQuestResponse* response);

    void RestartEventQuestImpl(UserContext &user,
                           const ::apb::api::quest::RestartEventQuestRequest* request,
                        ::apb::api::quest::RestartEventQuestResponse* response);

    void FinishEventQuestImpl(UserContext &user,
                              const ::apb::api::quest::FinishEventQuestRequest* request,
                        ::apb::api::quest::FinishEventQuestResponse* response);

    void SetRouteImpl(
        UserContext &user,
        const ::apb::api::quest::SetRouteRequest* request,
        ::apb::api::quest::SetRouteResponse* response);

    void SetQuestSceneChoiceImpl(UserContext &user,
                        const ::apb::api::quest::SetQuestSceneChoiceRequest* request,
                        ::apb::api::quest::SetQuestSceneChoiceResponse* response);

    void FinishAutoOrbitImpl(UserContext &user,
                             const ::google::protobuf::Empty* request,
                             ::apb::api::quest::FinishAutoOrbitResponse* response);

    void recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber);
};

#endif


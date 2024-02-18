#ifndef SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H

#include <service/QuestService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class QuestService final : public apb::api::quest::QuestService::Service, public CommonService {
public:
    explicit QuestService(Database &db);
    ~QuestService();

    QuestService(const QuestService &other) = delete;
    QuestService &operator =(const QuestService &other) = delete;

    ::grpc::Status UpdateMainFlowSceneProgress(::grpc::ServerContext* context,
                                               const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                               ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) override;

    ::grpc::Status StartMainQuest(::grpc::ServerContext* context,
                                  const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response) override;

    ::grpc::Status FinishMainQuest(::grpc::ServerContext* context, const ::apb::api::quest::FinishMainQuestRequest* request,
                                   ::apb::api::quest::FinishMainQuestResponse* response) override;

    ::grpc::Status UpdateMainQuestSceneProgress(::grpc::ServerContext* context, const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) override;

    void issueAllFirstClearRewards();

private:

    enum QuestStateType : int32_t {
        QuestStateType_MainFlowInProgress = 1, // Verified
        QuestStateType_MainFlowComplete = 2 // Verified
    };

    enum QuestFlowType : int32_t {
        QuestFlowType_NoFlow = 0, // Verified
        QuestFlowType_MainFlow = 1, // Verified
    };

    void setMainQuestProgressStatus(int64_t userId, int32_t currentQuestSceneId, int32_t headQuestSceneId, QuestFlowType currentQuestFlowType);
    void setMainQuestFlowStatus(int64_t userId, QuestFlowType flowType);

    void UpdateMainFlowSceneProgressImpl(int64_t userId,
                                         const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                         ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response);

    void StartMainQuestImpl(int64_t userId, const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response);

    void FinishMainQuestImpl(int64_t userId, const ::apb::api::quest::FinishMainQuestRequest* request,
                             ::apb::api::quest::FinishMainQuestResponse* response);

    void UpdateMainQuestSceneProgressImpl(int64_t userId,
                                          const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
                                          ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response);

    void updateMainQuestProgress(int64_t userId);

    void issueFirstClearRewardGroup(int64_t userId, int64_t firstClearGroupId);
};

#endif


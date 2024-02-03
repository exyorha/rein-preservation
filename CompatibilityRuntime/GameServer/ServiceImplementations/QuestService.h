#ifndef SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H

#include <service/QuestService.grpc.pb.h>

class QuestService final : public apb::api::quest::QuestService::Service {
public:
    QuestService();
    ~QuestService();

    QuestService(const QuestService &other) = delete;
    QuestService &operator =(const QuestService &other) = delete;

    ::grpc::Status UpdateMainFlowSceneProgress(::grpc::ServerContext* context,
                                               const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                               ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) override;

    ::grpc::Status StartMainQuest(::grpc::ServerContext* context,
                                  const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response) override;

    ::grpc::Status FinishMainQuest(::grpc::ServerContext* context, const ::apb::api::quest::FinishMainQuestRequest* request, ::apb::api::quest::FinishMainQuestResponse* response) override;

    ::grpc::Status UpdateMainQuestSceneProgress(::grpc::ServerContext* context, const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) override;

};

#endif


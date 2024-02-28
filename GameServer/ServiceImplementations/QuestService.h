#ifndef SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_QUEST_SERVICE_H

#include "service/QuestService.pb.h"
#include <google/protobuf/reflection.h>
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

    ::grpc::Status RestartMainQuest(::grpc::ServerContext* context,
                                    const ::apb::api::quest::RestartMainQuestRequest* request, ::apb::api::quest::RestartMainQuestResponse* response) override;

    ::grpc::Status FinishMainQuest(::grpc::ServerContext* context, const ::apb::api::quest::FinishMainQuestRequest* request,
                                   ::apb::api::quest::FinishMainQuestResponse* response) override;

    ::grpc::Status SetRoute(
        ::grpc::ServerContext* context,
        const ::apb::api::quest::SetRouteRequest* request,
        ::apb::api::quest::SetRouteResponse* response) override;

    ::grpc::Status UpdateMainQuestSceneProgress(
        ::grpc::ServerContext* context,
        const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request,
        ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) override;

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

    void recordQuestStartAttributes(int32_t questId, int32_t userDeckNumber);
};

#endif


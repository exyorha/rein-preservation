#include "QuestService.h"

QuestService::QuestService() = default;

QuestService::~QuestService() = default;


::grpc::Status QuestService::UpdateMainFlowSceneProgress(::grpc::ServerContext* context,
                                            const ::apb::api::quest::UpdateMainFlowSceneProgressRequest* request,
                                            ::apb::api::quest::UpdateMainFlowSceneProgressResponse* response) {

    printf("QuestService::UpdateMainFlowSceneProgress: %s\n", request->DebugString().c_str());

    std::stringstream stream;
    stream << R"json([{"userId":1,"currentMainQuestRouteId":1,"currentQuestSceneId":)json";
    stream << request->quest_scene_id();
    stream << ",\"headQuestSceneId\":" << request->quest_scene_id();
    stream << R"json(,"isReachedLastQuestScene":false,"latestVersion":1}])json";

    auto &mainQuestMainFlow = (*response->mutable_diff_user_data())["IUserMainQuestMainFlowStatus"];
    mainQuestMainFlow.set_update_records_json(stream.str());
    mainQuestMainFlow.set_delete_keys_json("[]");

    return grpc::Status::OK;
}


::grpc::Status QuestService::StartMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::StartMainQuestRequest* request, ::apb::api::quest::StartMainQuestResponse* response) {

    printf("QuestService::StartMainQuest: %s\n", request->DebugString().c_str());

#if 0
    response->mutable_diff_user_data()->emplace("IUserMainQuestMainFlowStatus", stream.str());
#endif
#if 0

    response->mutable_diff_user_data()->emplace(
        "IUserMainQuestFlowStatus",
        R"json([{"userId":1,"currentQuestFlowType":1,"latestVersion":1}])json"
    );
#endif

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

    return grpc::Status::OK;
}


::grpc::Status QuestService::FinishMainQuest(::grpc::ServerContext* context,
        const ::apb::api::quest::FinishMainQuestRequest* request, ::apb::api::quest::FinishMainQuestResponse* response) {

    printf("QuestService::FinishMainQuest: %s\n", request->DebugString().c_str());

    return grpc::Status::OK;

}

::grpc::Status QuestService::UpdateMainQuestSceneProgress(::grpc::ServerContext* context, const ::apb::api::quest::UpdateMainQuestSceneProgressRequest* request, ::apb::api::quest::UpdateMainQuestSceneProgressResponse* response) {
    printf("QuestService::UpdateMainQuestSceneProgress: %s\n", request->DebugString().c_str());
    auto &mainQuestProgress = (*response->mutable_diff_user_data())["IUserMainQuestProgressStatus"];

    std::stringstream stream;
    stream << R"json([{"userId":1,"currentQuestSceneId":)json";
    stream << request->quest_scene_id();
    stream << R"json(,"headQuestSceneId":)json";
    stream << request->quest_scene_id();
    stream << R"json(,"currentQuestFlowType":1,"latestVersion":1}])json";

    mainQuestProgress.set_update_records_json(stream.str());
    mainQuestProgress.set_delete_keys_json("[]");

    return grpc::Status::OK;
}

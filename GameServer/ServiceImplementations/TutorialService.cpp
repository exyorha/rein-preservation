#include <ServiceImplementations/TutorialService.h>

#include <DataModel/Sqlite/Statement.h>

TutorialService::TutorialService(Database &db) : CommonService(db) {

}

TutorialService::~TutorialService() = default;

::grpc::Status TutorialService::SetTutorialProgress(
    ::grpc::ServerContext* context, const ::apb::api::tutorial::SetTutorialProgressRequest* request,
    ::apb::api::tutorial::SetTutorialProgressResponse* response) {

    return inChangesetCall("TutorialService::SetTutorialProgress", context, request, response, &TutorialService::SetTutorialProgressImpl);
}

::grpc::Status TutorialService::SetTutorialProgressAndReplaceDeck(
    ::grpc::ServerContext* context, const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
    ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response) {

    return inChangesetCall("TutorialService::SetTutorialProgressAndReplaceDeck", context, request, response,
                           &TutorialService::SetTutorialProgressAndReplaceDeckImpl);
}

void TutorialService::SetTutorialProgressImpl(
    UserContext &user, const ::apb::api::tutorial::SetTutorialProgressRequest* request,
    ::apb::api::tutorial::SetTutorialProgressResponse* response) {

    user.setUserTutorialProgress(request->tutorial_type(), request->progress_phase(), request->choice_id());
}

void TutorialService::SetTutorialProgressAndReplaceDeckImpl(
    UserContext &user, const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
    ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response) {

    user.setUserTutorialProgress(request->tutorial_type(), request->progress_phase());

    user.replaceDeck(request->deck_type(), request->user_deck_number(),
                request->has_deck() ? &request->deck() : nullptr);
}

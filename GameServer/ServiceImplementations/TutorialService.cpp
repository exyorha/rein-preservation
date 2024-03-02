#include <ServiceImplementations/TutorialService.h>

#include <DataModel/Sqlite/Statement.h>

TutorialService::TutorialService(Database &db) : CommonService(db) {

}

TutorialService::~TutorialService() = default;

void TutorialService::SetTutorialProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::tutorial::SetTutorialProgressRequest* request,
                        ::apb::api::tutorial::SetTutorialProgressResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inChangesetCall("TutorialService::SetTutorialProgress", controller, request, response,
                           done, &TutorialService::SetTutorialProgressImpl);
}

void TutorialService::SetTutorialProgressAndReplaceDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
                        ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("TutorialService::SetTutorialProgressAndReplaceDeck", controller, request, response,
                           done, &TutorialService::SetTutorialProgressAndReplaceDeckImpl);
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

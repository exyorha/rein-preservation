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
    int64_t userId, const ::apb::api::tutorial::SetTutorialProgressRequest* request,
    ::apb::api::tutorial::SetTutorialProgressResponse* response) {

    setUserTutorialProgress(userId, request->tutorial_type(), request->progress_phase(), request->choice_id());
}

void TutorialService::SetTutorialProgressAndReplaceDeckImpl(
    int64_t userId, const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
    ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response) {

    setUserTutorialProgress(userId, request->tutorial_type(), request->progress_phase());

    replaceDeck(userId, request->deck_type(), request->user_deck_number(),
                request->has_deck() ? &request->deck() : nullptr);
}

void TutorialService::setUserTutorialProgress(int64_t userId, int32_t tutorialType, int32_t progressPhase, int32_t choiceId) {

    auto statement = db().prepare(R"SQL(
        INSERT INTO i_user_tutorial_progress (
            user_id,
            tutorial_type,
            progress_phase,
            choice_id
        ) VALUES (
            ?,
            ?,
            ?,
            ?
        )
        ON CONFLICT DO UPDATE SET
            progress_phase = excluded.progress_phase,
            choice_id =
                CASE
                    WHEN excluded.choice_id != 0 THEN excluded.choice_id
                    ELSE choice_id
                END
    )SQL");

    statement->bind(1, userId);
    statement->bind(2, tutorialType);
    statement->bind(3, progressPhase);
    statement->bind(4, choiceId);
    statement->exec();
}


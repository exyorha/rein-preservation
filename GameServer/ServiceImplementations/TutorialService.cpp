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

void TutorialService::SetTutorialProgressImpl(
    int64_t userId, const ::apb::api::tutorial::SetTutorialProgressRequest* request,
    ::apb::api::tutorial::SetTutorialProgressResponse* response) {


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
choice_id = excluded.choice_id
    )SQL");

    statement->bind(1, userId);
    statement->bind(2, request->tutorial_type());
    statement->bind(3, request->progress_phase());
    statement->bind(4, request->choice_id());
    statement->exec();
}

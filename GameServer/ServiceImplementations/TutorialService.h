#ifndef SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H

#include <service/TutorialService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are present.
 */
class TutorialService final : public apb::api::tutorial::TutorialService::Service, public CommonService {
public:
    explicit TutorialService(Database &db);
    ~TutorialService();

    TutorialService(const TutorialService &other) = delete;
    TutorialService &operator =(const TutorialService &other) = delete;

    ::grpc::Status SetTutorialProgress(::grpc::ServerContext* context,
                                       const ::apb::api::tutorial::SetTutorialProgressRequest* request, ::apb::api::tutorial::SetTutorialProgressResponse* response) override;

    ::grpc::Status SetTutorialProgressAndReplaceDeck(::grpc::ServerContext* context,
                                       const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
                                       ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response) override;

private:
    void SetTutorialProgressImpl(UserContext &user,
                                 const ::apb::api::tutorial::SetTutorialProgressRequest* request,
                                 ::apb::api::tutorial::SetTutorialProgressResponse* response);

    void SetTutorialProgressAndReplaceDeckImpl(UserContext &user,
                                 const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
                                 ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response);
};

#endif



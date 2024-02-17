#ifndef SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H

#include <service/TutorialService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class TutorialService final : public apb::api::tutorial::TutorialService::Service, public CommonService {
public:
    explicit TutorialService(Database &db);
    ~TutorialService();

    TutorialService(const TutorialService &other) = delete;
    TutorialService &operator =(const TutorialService &other) = delete;

    ::grpc::Status SetTutorialProgress(::grpc::ServerContext* context,
                                       const ::apb::api::tutorial::SetTutorialProgressRequest* request, ::apb::api::tutorial::SetTutorialProgressResponse* response) override;
private:
    void SetTutorialProgressImpl(int64_t userId,
                                 const ::apb::api::tutorial::SetTutorialProgressRequest* request,
                                 ::apb::api::tutorial::SetTutorialProgressResponse* response);
};

#endif



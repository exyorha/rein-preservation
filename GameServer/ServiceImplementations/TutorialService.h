#ifndef SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_TUTORIAL_SERVICE_H

#include <service/TutorialService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are present.
 */
class TutorialService final : public apb::api::tutorial::TutorialService, public CommonService {
public:
    explicit TutorialService(Database &db);
    ~TutorialService();

    TutorialService(const TutorialService &other) = delete;
    TutorialService &operator =(const TutorialService &other) = delete;

    void SetTutorialProgress(::google::protobuf::RpcController* controller,
                        const ::apb::api::tutorial::SetTutorialProgressRequest* request,
                        ::apb::api::tutorial::SetTutorialProgressResponse* response,
                        ::google::protobuf::Closure* done) override;

    void SetTutorialProgressAndReplaceDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
                        ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void SetTutorialProgressImpl(UserContext &user,
                                 const ::apb::api::tutorial::SetTutorialProgressRequest* request,
                                 ::apb::api::tutorial::SetTutorialProgressResponse* response);

    void SetTutorialProgressAndReplaceDeckImpl(UserContext &user,
                                 const ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckRequest* request,
                                 ::apb::api::tutorial::SetTutorialProgressAndReplaceDeckResponse* response);
};

#endif



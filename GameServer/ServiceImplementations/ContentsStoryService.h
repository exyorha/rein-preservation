#ifndef SERVICE_IMPLEMENTATIONS_CONTENTS_STORY_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CONTENTS_STORY_SERVICE_H

#include <service/ContentsStoryService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class ContentsStoryService final : public apb::api::contentsstory::ContentsStoryService, public CommonService {
public:
    explicit ContentsStoryService(Database &db);
    ~ContentsStoryService();

    ContentsStoryService(const ContentsStoryService &other) = delete;
    ContentsStoryService &operator =(const ContentsStoryService &other) = delete;

    void RegisterPlayed(::google::protobuf::RpcController* controller,
                        const ::apb::api::contentsstory::RegisterPlayedRequest* request,
                        ::apb::api::contentsstory::RegisterPlayedResponse* response,
                        ::google::protobuf::Closure* done) override;

private:

    void RegisterPlayedImpl(
        UserContext &user, const ::apb::api::contentsstory::RegisterPlayedRequest* request,
        ::apb::api::contentsstory::RegisterPlayedResponse* response);

};

#endif





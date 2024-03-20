#include <ServiceImplementations/ContentsStoryService.h>

ContentsStoryService::ContentsStoryService(Database &db) : CommonService(db) {

}

ContentsStoryService::~ContentsStoryService() = default;

void ContentsStoryService::RegisterPlayed(::google::protobuf::RpcController* controller,
                        const ::apb::api::contentsstory::RegisterPlayedRequest* request,
                        ::apb::api::contentsstory::RegisterPlayedResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("ContentsStoryService::RegisterPlayed", controller, request, response, done, &ContentsStoryService::RegisterPlayedImpl);
}


void ContentsStoryService::RegisterPlayedImpl(
    UserContext &user, const ::apb::api::contentsstory::RegisterPlayedRequest* request,
    ::apb::api::contentsstory::RegisterPlayedResponse* response) {


    user.registerContentsStoryPlayed(request->contents_story_id());
}

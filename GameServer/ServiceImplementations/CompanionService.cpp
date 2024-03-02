#include <ServiceImplementations/CompanionService.h>

CompanionService::CompanionService(Database &db) : CommonService(db) {

}

CompanionService::~CompanionService() = default;

void CompanionService::Enhance(::google::protobuf::RpcController* controller,
                        const ::apb::api::companion::EnhanceRequest* request,
                        ::apb::api::companion::EnhanceResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CompanionService::Enhance", controller, request, response, done, &CompanionService::EnhanceImpl);
}


void CompanionService::EnhanceImpl(
    UserContext &user, const ::apb::api::companion::EnhanceRequest* request,
    ::apb::api::companion::EnhanceResponse* response) {

    if(request->add_level_count() < 0)
        throw std::logic_error("bad level count");

    for(int32_t levelToAdd = 0; levelToAdd < request->add_level_count(); levelToAdd++) {
        user.enhanceCompanion(request->user_companion_uuid());
    }
}

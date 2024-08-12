#include <ServiceImplementations/CharacterViewerService.h>

CharacterViewerService::CharacterViewerService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

CharacterViewerService::~CharacterViewerService() = default;


void CharacterViewerService::CharacterViewerTop(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::characterviewer::CharacterViewerTopResponse* response,
                        ::google::protobuf::Closure* done)  {

    return inChangesetCall("CharacterViewerService::CharacterViewerTop", controller, request, response, done,
                           &CharacterViewerService::CharacterViewerTopImpl);

}

void CharacterViewerService::CharacterViewerTopImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request,
    ::apb::api::characterviewer::CharacterViewerTopResponse* response) {

    // TODO
}


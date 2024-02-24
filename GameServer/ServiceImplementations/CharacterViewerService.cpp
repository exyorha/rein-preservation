#include <ServiceImplementations/CharacterViewerService.h>

CharacterViewerService::CharacterViewerService(Database &db) :CommonService(db) {

}

CharacterViewerService::~CharacterViewerService() = default;


::grpc::Status CharacterViewerService::CharacterViewerTop(
    ::grpc::ServerContext* context,
    const ::google::protobuf::Empty* request,
    ::apb::api::characterviewer::CharacterViewerTopResponse* response) {

    return inChangesetCall("CharacterViewerService::CharacterViewerTop", context, request, response,  &CharacterViewerService::CharacterViewerTopImpl);

}

void CharacterViewerService::CharacterViewerTopImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request,
    ::apb::api::characterviewer::CharacterViewerTopResponse* response) {

    // TODO
}


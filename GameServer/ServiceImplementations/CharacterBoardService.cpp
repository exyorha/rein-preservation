#include <ServiceImplementations/CharacterBoardService.h>

CharacterBoardService::CharacterBoardService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

CharacterBoardService::~CharacterBoardService() = default;

void CharacterBoardService::ReleasePanel(::google::protobuf::RpcController* controller,
                        const ::apb::api::characterboard::ReleasePanelRequest* request,
                        ::apb::api::characterboard::ReleasePanelResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CharacterBoardService::ReleasePanel", controller, request, response, done,
                           &CharacterBoardService::ReleasePanelImpl);
}

void CharacterBoardService::ReleasePanelImpl(UserContext &userContext, const ::apb::api::characterboard::ReleasePanelRequest* request,
                                             ::apb::api::characterboard::ReleasePanelResponse* response) {

    UserContext::CharacterBoardDeferredUpdate update;

    for(auto id: request->character_board_panel_id()) {
        userContext.releaseCharacterBoardPanel(id, update);
    }

    userContext.finalizeCharacterBoardUpdate(update);
}

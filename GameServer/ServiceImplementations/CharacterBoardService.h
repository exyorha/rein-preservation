#ifndef SERVICE_IMPLEMENTATIONS_CHARACTER_BOARD_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CHARACTER_BOARD_SERVICE_H

#include <service/CharacterBoardService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class CharacterBoardService final : public apb::api::characterboard::CharacterBoardService, public CommonService {
public:
    CharacterBoardService(Database &db, const GameServerConfiguration &config);
    ~CharacterBoardService();

    CharacterBoardService(const CharacterBoardService &other) = delete;
    CharacterBoardService &operator =(const CharacterBoardService &other) = delete;

    void ReleasePanel(::google::protobuf::RpcController* controller,
                        const ::apb::api::characterboard::ReleasePanelRequest* request,
                        ::apb::api::characterboard::ReleasePanelResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void ReleasePanelImpl(UserContext &userContext,
                          const ::apb::api::characterboard::ReleasePanelRequest* request,
                          ::apb::api::characterboard::ReleasePanelResponse* response);

};

#endif





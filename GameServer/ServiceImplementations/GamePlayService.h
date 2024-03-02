#ifndef SERVICE_IMPLEMENTATIONS_GAMEPLAY_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GAMEPLAY_SERVICE_H

#include <service/GamePlayService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are present.
 */
class GamePlayService final : public apb::api::gameplay::GamePlayService, public CommonService {
public:
    explicit GamePlayService(Database &db);
    ~GamePlayService();

    GamePlayService(const GamePlayService &other) = delete;
    GamePlayService &operator =(const GamePlayService &other) = delete;

    void CheckBeforeGamePlay(::google::protobuf::RpcController* controller,
                            const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                            ::apb::api::gameplay::CheckBeforeGamePlayResponse* response,
                            ::google::protobuf::Closure* done) override;

private:
    void CheckBeforeGamePlayImpl(UserContext &user,
                                 const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                 ::apb::api::gameplay::CheckBeforeGamePlayResponse* response);

};

#endif

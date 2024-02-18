#ifndef SERVICE_IMPLEMENTATIONS_GAMEPLAY_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GAMEPLAY_SERVICE_H

#include <service/GamePlayService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are present.
 */
class GamePlayService final : public apb::api::gameplay::GamePlayService::Service, public CommonService {
public:
    explicit GamePlayService(Database &db);
    ~GamePlayService();

    GamePlayService(const GamePlayService &other) = delete;
    GamePlayService &operator =(const GamePlayService &other) = delete;

    ::grpc::Status CheckBeforeGamePlay(::grpc::ServerContext* context,
                                       const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                       ::apb::api::gameplay::CheckBeforeGamePlayResponse* response) override;

private:
    void CheckBeforeGamePlayImpl(int64_t userId,
                                 const ::apb::api::gameplay::CheckBeforeGamePlayRequest* request,
                                 ::apb::api::gameplay::CheckBeforeGamePlayResponse* response);

};

#endif

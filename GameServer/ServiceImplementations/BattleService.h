#ifndef SERVICE_IMPLEMENTATIONS_BATTLE_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_BATTLE_SERVICE_H

#include <service/BattleService.pb.h>

#include <ServiceImplementations/CommonService.h>

class BattleService final : public apb::api::battle::BattleService, public CommonService {
public:
    BattleService(Database &db, const GameServerConfiguration &config);
    ~BattleService();

    BattleService(const BattleService &other) = delete;
    BattleService &operator =(const BattleService &other) = delete;

    void StartWave(::google::protobuf::RpcController* controller,
                        const ::apb::api::battle::StartWaveRequest* request,
                        ::apb::api::battle::StartWaveResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishWave(::google::protobuf::RpcController* controller,
                        const ::apb::api::battle::FinishWaveRequest* request,
                        ::apb::api::battle::FinishWaveResponse* response,
                        ::google::protobuf::Closure* done) override;
private:
    void StartWaveImpl(UserContext &user,
                           const ::apb::api::battle::StartWaveRequest* request,
                           ::apb::api::battle::StartWaveResponse* response);

    void FinishWaveImpl(UserContext &user,
                           const ::apb::api::battle::FinishWaveRequest* request,
                           ::apb::api::battle::FinishWaveResponse* response);
};

#endif





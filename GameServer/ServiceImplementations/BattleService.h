#ifndef SERVICE_IMPLEMENTATIONS_BATTLE_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_BATTLE_SERVICE_H

#include <service/BattleService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class BattleService final : public apb::api::battle::BattleService::Service, public CommonService {
public:
    explicit BattleService(Database &db);
    ~BattleService();

    BattleService(const BattleService &other) = delete;
    BattleService &operator =(const BattleService &other) = delete;

    ::grpc::Status StartWave(::grpc::ServerContext* context,
                                 const ::apb::api::battle::StartWaveRequest* request,
                                 ::apb::api::battle::StartWaveResponse* response) override;

    ::grpc::Status FinishWave(::grpc::ServerContext* context,
                                 const ::apb::api::battle::FinishWaveRequest* request,
                                 ::apb::api::battle::FinishWaveResponse* response) override;
private:
    void StartWaveImpl(int64_t userId,
                           const ::apb::api::battle::StartWaveRequest* request,
                           ::apb::api::battle::StartWaveResponse* response);

    void FinishWaveImpl(int64_t userId,
                           const ::apb::api::battle::FinishWaveRequest* request,
                           ::apb::api::battle::FinishWaveResponse* response);
};

#endif





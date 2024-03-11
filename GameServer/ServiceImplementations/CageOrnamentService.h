#ifndef SERVICE_IMPLEMENTATIONS_CAGE_ORNAMENT_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CAGE_ORNAMENT_SERVICE_H

#include <service/CageOrnamentService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is complete.
 */
class CageOrnamentService final : public apb::api::cageornament::CageOrnamentService, public CommonService {
public:
    explicit CageOrnamentService(Database &db);
    ~CageOrnamentService();

    CageOrnamentService(const CageOrnamentService &other) = delete;
    CageOrnamentService &operator =(const CageOrnamentService &other) = delete;

    void ReceiveReward(::google::protobuf::RpcController* controller,
                        const ::apb::api::cageornament::ReceiveRewardRequest* request,
                        ::apb::api::cageornament::ReceiveRewardResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RecordAccess(::google::protobuf::RpcController* controller,
                        const ::apb::api::cageornament::RecordAccessRequest* request,
                        ::apb::api::cageornament::RecordAccessResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void ReceiveRewardImpl(
        UserContext &user,
        const ::apb::api::cageornament::ReceiveRewardRequest* request,
        ::apb::api::cageornament::ReceiveRewardResponse* response);

    void RecordAccessImpl(
        UserContext &user,
        const ::apb::api::cageornament::RecordAccessRequest* request,
        ::apb::api::cageornament::RecordAccessResponse* response);

};

#endif






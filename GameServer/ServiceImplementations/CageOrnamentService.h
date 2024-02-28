#ifndef SERVICE_IMPLEMENTATIONS_CAGE_ORNAMENT_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CAGE_ORNAMENT_SERVICE_H

#include <service/CageOrnamentService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class CageOrnamentService final : public apb::api::cageornament::CageOrnamentService::Service, public CommonService {
public:
    explicit CageOrnamentService(Database &db);
    ~CageOrnamentService();

    CageOrnamentService(const CageOrnamentService &other) = delete;
    CageOrnamentService &operator =(const CageOrnamentService &other) = delete;

    ::grpc::Status RecordAccess(
        ::grpc::ServerContext* context, const ::apb::api::cageornament::RecordAccessRequest* request,
        ::apb::api::cageornament::RecordAccessResponse* response) override;

private:
    void RecordAccessImpl(
        UserContext &user,
        const ::apb::api::cageornament::RecordAccessRequest* request,
        ::apb::api::cageornament::RecordAccessResponse* response);

};

#endif






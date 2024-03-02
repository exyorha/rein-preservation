#ifndef SERVICE_IMPLEMENTATIONS_COMPANION_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COMPANION_SERVICE_H

#include <service/CompanionService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class CompanionService final : public apb::api::companion::CompanionService, public CommonService {
public:
    explicit CompanionService(Database &db);
    ~CompanionService();

    CompanionService(const CompanionService &other) = delete;
    CompanionService &operator =(const CompanionService &other) = delete;

    void Enhance(::google::protobuf::RpcController* controller,
                        const ::apb::api::companion::EnhanceRequest* request,
                        ::apb::api::companion::EnhanceResponse* response,
                        ::google::protobuf::Closure* done) override;

private:

    void EnhanceImpl(
        UserContext &user, const ::apb::api::companion::EnhanceRequest* request,
        ::apb::api::companion::EnhanceResponse* response);

};

#endif





#ifndef SERVICE_IMPLEMENTATIONS_EXPLORE_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_EXPLORE_SERVICE_H

#include <service/ExploreService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is complete.
 */
class ExploreService final : public apb::api::explore::ExploreService, public CommonService {
public:
    ExploreService(Database &db, const GameServerConfiguration &config);
    ~ExploreService();

    ExploreService(const ExploreService &other) = delete;
    ExploreService &operator =(const ExploreService &other) = delete;

    void StartExplore(::google::protobuf::RpcController* controller,
                        const ::apb::api::explore::StartExploreRequest* request,
                        ::apb::api::explore::StartExploreResponse* response,
                        ::google::protobuf::Closure* done) override;

    void FinishExplore(::google::protobuf::RpcController* controller,
                        const ::apb::api::explore::FinishExploreRequest* request,
                        ::apb::api::explore::FinishExploreResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RetireExplore(::google::protobuf::RpcController* controller,
                        const ::apb::api::explore::RetireExploreRequest* request,
                        ::apb::api::explore::RetireExploreResponse* response,
                        ::google::protobuf::Closure* done) override;
private:

    void StartExploreImpl(UserContext &user,
                          const ::apb::api::explore::StartExploreRequest* request,
                          ::apb::api::explore::StartExploreResponse* response);

    void FinishExploreImpl(UserContext &user,
                           const ::apb::api::explore::FinishExploreRequest* request,
                          ::apb::api::explore::FinishExploreResponse* response);

    void RetireExploreImpl(UserContext &user,
                           const ::apb::api::explore::RetireExploreRequest* request,
                        ::apb::api::explore::RetireExploreResponse* response);
};

#endif

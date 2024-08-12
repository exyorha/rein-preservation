#ifndef SERVICE_IMPLEMENTATIONS_LABYRINTH_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_LABYRINTH_SERVICE_H

#include <service/LabyrinthService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is complete.
 */
class LabyrinthService final : public apb::api::labyrinth::LabyrinthService, public CommonService {
public:
    LabyrinthService(Database &db, const GameServerConfiguration &config);
    ~LabyrinthService();

    LabyrinthService(const LabyrinthService &other) = delete;
    LabyrinthService &operator =(const LabyrinthService &other) = delete;

    void UpdateSeasonData(::google::protobuf::RpcController* controller,
                        const ::apb::api::labyrinth::UpdateSeasonDataRequest* request,
                        ::apb::api::labyrinth::UpdateSeasonDataResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void UpdateSeasonDataImpl(UserContext &user,
                              const ::apb::api::labyrinth::UpdateSeasonDataRequest* request,
                              ::apb::api::labyrinth::UpdateSeasonDataResponse* response);
};

#endif

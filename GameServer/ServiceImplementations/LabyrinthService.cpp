#include "ServiceImplementations/LabyrinthService.h"

LabyrinthService::LabyrinthService(Database &db) : CommonService(db) {

}

LabyrinthService::~LabyrinthService() = default;

void LabyrinthService::UpdateSeasonData(::google::protobuf::RpcController* controller,
                    const ::apb::api::labyrinth::UpdateSeasonDataRequest* request,
                    ::apb::api::labyrinth::UpdateSeasonDataResponse* response,
                    ::google::protobuf::Closure* done) {

    inChangesetCall("LabyrinthService::UpdateSeasonData", controller, request, response, done, &LabyrinthService::UpdateSeasonDataImpl);

}

void LabyrinthService::UpdateSeasonDataImpl(UserContext &user,
                    const ::apb::api::labyrinth::UpdateSeasonDataRequest* request,
                    ::apb::api::labyrinth::UpdateSeasonDataResponse* response) {

    // TODO
}

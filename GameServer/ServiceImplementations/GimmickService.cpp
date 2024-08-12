#include "GimmickService.h"
#include "service/GimmickService.pb.h"

#include <cstdio>
#include <google/protobuf/repeated_ptr_field.h>

GimmickService::GimmickService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

GimmickService::~GimmickService() = default;

void GimmickService::UpdateSequence(::google::protobuf::RpcController* controller,
                    const ::apb::api::gimmick::UpdateSequenceRequest* request,
                    ::apb::api::gimmick::UpdateSequenceResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GimmickService::UpdateSequence", controller, request, response, done, &GimmickService::UpdateSequenceImpl);
}

void GimmickService::UpdateGimmickProgress(::google::protobuf::RpcController* controller,
                    const ::apb::api::gimmick::UpdateGimmickProgressRequest* request,
                    ::apb::api::gimmick::UpdateGimmickProgressResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GimmickService::UpdateGimmickProgress", controller, request, response, done, &GimmickService::UpdateGimmickProgressImpl);
}

void GimmickService::InitSequenceSchedule(::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Empty* request,
                            ::apb::api::gimmick::InitSequenceScheduleResponse* response,
                            ::google::protobuf::Closure* done) {

    return inChangesetCall("GimmickService::InitSequenceSchedule", controller, request, response,
                           done, &GimmickService::InitSequenceScheduleImpl);
}

void GimmickService::Unlock(::google::protobuf::RpcController* controller,
                    const ::apb::api::gimmick::UnlockRequest* request,
                    ::apb::api::gimmick::UnlockResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GimmickService::Unlock", controller, request, response, done, &GimmickService::UnlockImpl);
}

void GimmickService::UpdateSequenceImpl(UserContext &user,
                        const ::apb::api::gimmick::UpdateSequenceRequest* request,
                        ::apb::api::gimmick::UpdateSequenceResponse* response) {

    user.updateGimmickSequence(request->gimmick_sequence_schedule_id(), request->gimmick_sequence_id());
}


void GimmickService::UpdateGimmickProgressImpl(UserContext &user,
                        const ::apb::api::gimmick::UpdateGimmickProgressRequest* request,
                        ::apb::api::gimmick::UpdateGimmickProgressResponse* response) {

    bool sequenceCleared;

    user.updateGimmickProgress(
        request->gimmick_sequence_schedule_id(),
        request->gimmick_sequence_id(),
        request->gimmick_id(),
        request->gimmick_ornament_index(),
        request->progress_value_bit(),
        response->mutable_gimmick_ornament_reward(),
        sequenceCleared,
        response->mutable_gimmick_sequence_clear_reward());

    response->set_is_sequence_cleared(sequenceCleared);
}

void GimmickService::InitSequenceScheduleImpl(
    UserContext &user,
    const ::google::protobuf::Empty* request, ::apb::api::gimmick::InitSequenceScheduleResponse* response) {

    user.initGimmickSequenceSchedule();
}


void GimmickService::UnlockImpl(UserContext &user, const ::apb::api::gimmick::UnlockRequest* request, ::apb::api::gimmick::UnlockResponse* response) {

    for(const auto &gimmick: request->gimmick_key()) {

        user.unlockGimmick(gimmick.gimmick_sequence_schedule_id(), gimmick.gimmick_sequence_id(), gimmick.gimmick_id());
    }
}


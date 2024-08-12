#include <ServiceImplementations/CharacterService.h>

CharacterService::CharacterService(Database &db, const GameServerConfiguration &config) : CommonService(db, config) {

}

CharacterService::~CharacterService() = default;

void CharacterService::Rebirth(::google::protobuf::RpcController* controller,
                        const ::apb::api::character::RebirthRequest* request,
                        ::apb::api::character::RebirthResponse* response,
                        ::google::protobuf::Closure* done) {

    return inChangesetCall("CharacterService::Rebirth", controller, request, response, done,
                           &CharacterService::RebirthImpl);
}


void CharacterService::RebirthImpl(UserContext &user, const ::apb::api::character::RebirthRequest* request,
                                   ::apb::api::character::RebirthResponse* response) {


    if(request->rebirth_count() < 0)
        throw std::logic_error("bad count");

    for(int32_t rebirth = 0; rebirth < request->rebirth_count(); rebirth++) {
        user.rebirthCharacter(request->character_id());
    }
}

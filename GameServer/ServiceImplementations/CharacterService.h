#ifndef SERVICE_IMPLEMENTATIONS_CHARACTER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CHARACTER_SERVICE_H

#include <service/CharacterService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class CharacterService final : public apb::api::character::CharacterService, public CommonService {
public:
    CharacterService(Database &db, const GameServerConfiguration &config);
    ~CharacterService();

    CharacterService(const CharacterService &other) = delete;
    CharacterService &operator =(const CharacterService &other) = delete;

    void Rebirth(::google::protobuf::RpcController* controller,
                 const ::apb::api::character::RebirthRequest* request,
                 ::apb::api::character::RebirthResponse* response,
                 ::google::protobuf::Closure* done) override;

private:
    void RebirthImpl(UserContext &user,
                     const ::apb::api::character::RebirthRequest* request,
                     ::apb::api::character::RebirthResponse* response);



};

#endif




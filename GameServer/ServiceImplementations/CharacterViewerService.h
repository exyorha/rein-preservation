#ifndef SERVICE_IMPLEMENTATIONS_CHARACTER_VIEWER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CHARACTER_VIEWER_SERVICE_H

#include <service/CharacterViewerService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class CharacterViewerService final : public apb::api::characterviewer::CharacterViewerService, public CommonService {
public:
    explicit CharacterViewerService(Database &db);
    ~CharacterViewerService();

    CharacterViewerService(const CharacterViewerService &other) = delete;
    CharacterViewerService &operator =(const CharacterViewerService &other) = delete;

    void CharacterViewerTop(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::characterviewer::CharacterViewerTopResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void CharacterViewerTopImpl(
        UserContext &user,
        const ::google::protobuf::Empty* request,
        ::apb::api::characterviewer::CharacterViewerTopResponse* response);
};

#endif



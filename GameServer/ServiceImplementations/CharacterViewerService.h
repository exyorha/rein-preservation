#ifndef SERVICE_IMPLEMENTATIONS_CHARACTER_VIEWER_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_CHARACTER_VIEWER_SERVICE_H

#include <service/CharacterViewerService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * All methods are defined.
 */
class CharacterViewerService final : public apb::api::characterviewer::CharacterViewerService::Service, public CommonService {
public:
    explicit CharacterViewerService(Database &db);
    ~CharacterViewerService();

    CharacterViewerService(const CharacterViewerService &other) = delete;
    CharacterViewerService &operator =(const CharacterViewerService &other) = delete;

    ::grpc::Status CharacterViewerTop(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::apb::api::characterviewer::CharacterViewerTopResponse* response) override;

private:
    void CharacterViewerTopImpl(
        int64_t userId,
        const ::google::protobuf::Empty* request,
        ::apb::api::characterviewer::CharacterViewerTopResponse* response);
};

#endif



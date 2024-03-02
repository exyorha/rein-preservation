#ifndef SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H

#include <service/DeckService.pb.h>

#include <ServiceImplementations/CommonService.h>

class DeckService final : public apb::api::deck::DeckService, public CommonService {
public:
    explicit DeckService(Database &db);
    ~DeckService();

    DeckService(const DeckService &other) = delete;
    DeckService &operator =(const DeckService &other) = delete;

    void UpdateName(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::UpdateNameRequest* request,
                        ::apb::api::deck::UpdateNameResponse* response,
                        ::google::protobuf::Closure* done) override;

    void ReplaceDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceDeckRequest* request,
                        ::apb::api::deck::ReplaceDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RefreshDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshDeckPowerRequest* request,
                        ::apb::api::deck::RefreshDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void UpdateNameImpl(UserContext &user,
                        const ::apb::api::deck::UpdateNameRequest* request,
                        ::apb::api::deck::UpdateNameResponse* response);

    void ReplaceDeckImpl(UserContext &user,
                         const ::apb::api::deck::ReplaceDeckRequest* request,
                         ::apb::api::deck::ReplaceDeckResponse* response);

    void RefreshDeckPowerImpl(UserContext &user,
                              const ::apb::api::deck::RefreshDeckPowerRequest* request,
                              ::apb::api::deck::RefreshDeckPowerResponse* response);
};

#endif



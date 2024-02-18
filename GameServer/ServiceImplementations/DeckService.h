#ifndef SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H

#include <service/DeckService.grpc.pb.h>

#include <ServiceImplementations/CommonService.h>

class DeckService final : public apb::api::deck::DeckService::Service, public CommonService {
public:
    explicit DeckService(Database &db);
    ~DeckService();

    DeckService(const DeckService &other) = delete;
    DeckService &operator =(const DeckService &other) = delete;

    ::grpc::Status UpdateName(::grpc::ServerContext* context,
                              const ::apb::api::deck::UpdateNameRequest* request,
                              ::apb::api::deck::UpdateNameResponse* response) override;

    ::grpc::Status ReplaceDeck(::grpc::ServerContext* context,
                               const ::apb::api::deck::ReplaceDeckRequest* request,
                               ::apb::api::deck::ReplaceDeckResponse* response) override;

    ::grpc::Status RefreshDeckPower(::grpc::ServerContext* context,
                                    const ::apb::api::deck::RefreshDeckPowerRequest* request,
                                    ::apb::api::deck::RefreshDeckPowerResponse* response) override;

private:
    void UpdateNameImpl(int64_t userId,
                        const ::apb::api::deck::UpdateNameRequest* request,
                        ::apb::api::deck::UpdateNameResponse* response);

    void ReplaceDeckImpl(int64_t userId,
                         const ::apb::api::deck::ReplaceDeckRequest* request,
                         ::apb::api::deck::ReplaceDeckResponse* response);

    void RefreshDeckPowerImpl(int64_t userId,
                              const ::apb::api::deck::RefreshDeckPowerRequest* request,
                              ::apb::api::deck::RefreshDeckPowerResponse* response);

    void updateDeckCharacterPower(int64_t userId, const apb::api::deck::DeckCharacterPower &power);

};

#endif



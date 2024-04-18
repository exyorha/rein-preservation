#ifndef SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_DECK_SERVICE_H

#include <service/DeckService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is fully implemented.
 */
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

    void SetPvpDefenseDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::SetPvpDefenseDeckRequest* request,
                        ::apb::api::deck::SetPvpDefenseDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

    void CopyDeck(::google::protobuf::RpcController* controller,
                  const ::apb::api::deck::CopyDeckRequest* request,
                  ::apb::api::deck::CopyDeckResponse* response,
                  ::google::protobuf::Closure* done) override;

    void RemoveDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RemoveDeckRequest* request,
                        ::apb::api::deck::RemoveDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RefreshDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshDeckPowerRequest* request,
                        ::apb::api::deck::RefreshDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) override;

    void UpdateTripleDeckName(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::UpdateTripleDeckNameRequest* request,
                        ::apb::api::deck::UpdateTripleDeckNameResponse* response,
                        ::google::protobuf::Closure* done) override;

    void ReplaceTripleDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceTripleDeckRequest* request,
                        ::apb::api::deck::ReplaceTripleDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

    void ReplaceMultiDeck(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::ReplaceMultiDeckRequest* request,
                        ::apb::api::deck::ReplaceMultiDeckResponse* response,
                        ::google::protobuf::Closure* done) override;

    void RefreshMultiDeckPower(::google::protobuf::RpcController* controller,
                        const ::apb::api::deck::RefreshMultiDeckPowerRequest* request,
                        ::apb::api::deck::RefreshMultiDeckPowerResponse* response,
                        ::google::protobuf::Closure* done) override;

private:
    void UpdateNameImpl(UserContext &user,
                        const ::apb::api::deck::UpdateNameRequest* request,
                        ::apb::api::deck::UpdateNameResponse* response);

    void ReplaceDeckImpl(UserContext &user,
                         const ::apb::api::deck::ReplaceDeckRequest* request,
                         ::apb::api::deck::ReplaceDeckResponse* response);

    void SetPvpDefenseDeckImpl(UserContext &user,
                           const ::apb::api::deck::SetPvpDefenseDeckRequest* request,
                        ::apb::api::deck::SetPvpDefenseDeckResponse* response);

    void CopyDeckImpl(UserContext &user,
                      const ::apb::api::deck::CopyDeckRequest* request,
                      ::apb::api::deck::CopyDeckResponse* response);

    void RemoveDeckImpl(UserContext &user,
                        const ::apb::api::deck::RemoveDeckRequest* request,
                        ::apb::api::deck::RemoveDeckResponse* response);

    void RefreshDeckPowerImpl(UserContext &user,
                              const ::apb::api::deck::RefreshDeckPowerRequest* request,
                              ::apb::api::deck::RefreshDeckPowerResponse* response);

    void UpdateTripleDeckNameImpl(UserContext &user,
                              const ::apb::api::deck::UpdateTripleDeckNameRequest* request,
                              ::apb::api::deck::UpdateTripleDeckNameResponse* response);

    void ReplaceTripleDeckImpl(UserContext &user,
                              const ::apb::api::deck::ReplaceTripleDeckRequest* request,
                              ::apb::api::deck::ReplaceTripleDeckResponse* response);

    void ReplaceMultiDeckImpl(UserContext &user,
                              const ::apb::api::deck::ReplaceMultiDeckRequest* request,
                        ::apb::api::deck::ReplaceMultiDeckResponse* response);

    void RefreshMultiDeckPowerImpl(UserContext &user,
                        const ::apb::api::deck::RefreshMultiDeckPowerRequest* request,
                        ::apb::api::deck::RefreshMultiDeckPowerResponse* response);
};

#endif



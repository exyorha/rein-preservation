#ifndef SERVICE_IMPLEMENTATIONS_GIFT_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_GIFT__SERVICE_H

#include <service/GiftService.pb.h>

#include <ServiceImplementations/CommonService.h>

/*
 * This service is complete.
 */
class GiftService final : public apb::api::gift::GiftService, public CommonService {
public:
    explicit GiftService(Database &db);
    ~GiftService();

    GiftService(const GiftService &other) = delete;
    GiftService &operator =(const GiftService &other) = delete;

    void ReceiveGift(::google::protobuf::RpcController* controller,
                        const ::apb::api::gift::ReceiveGiftRequest* request,
                        ::apb::api::gift::ReceiveGiftResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetGiftList(::google::protobuf::RpcController* controller,
                        const ::apb::api::gift::GetGiftListRequest* request,
                        ::apb::api::gift::GetGiftListResponse* response,
                        ::google::protobuf::Closure* done) override;

    void GetGiftReceiveHistoryList(::google::protobuf::RpcController* controller,
                        const ::google::protobuf::Empty* request,
                        ::apb::api::gift::GetGiftReceiveHistoryListResponse* response,
                        ::google::protobuf::Closure* done) override;
private:

    void ReceiveGiftImpl(UserContext &user,
                         const ::apb::api::gift::ReceiveGiftRequest* request,
                        ::apb::api::gift::ReceiveGiftResponse* response);

    void GetGiftListImpl(UserContext &user,
                     const ::apb::api::gift::GetGiftListRequest* request,
                        ::apb::api::gift::GetGiftListResponse* response);

    void GetGiftReceiveHistoryListImpl(UserContext &user,
                                       const ::google::protobuf::Empty* request,
                                       ::apb::api::gift::GetGiftReceiveHistoryListResponse* response);

    static void setTimestampFromNetTimestamp(google::protobuf::Timestamp *datetime, int64_t timestamp);

    std::unique_ptr<sqlite::Statement> runGiftListQuery(UserContext &user, const ::apb::api::gift::GetGiftListRequest* request, bool isTotalCount);
};

#endif



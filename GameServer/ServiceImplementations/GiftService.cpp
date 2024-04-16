#include "ServiceImplementations/GiftService.h"
#include "DataModel/DatabaseEnums.h"
#include "DataModel/Sqlite/Statement.h"

GiftService::GiftService(Database &db) : CommonService(db) {

}

GiftService::~GiftService() = default;

void GiftService::ReceiveGift(::google::protobuf::RpcController* controller,
                    const ::apb::api::gift::ReceiveGiftRequest* request,
                    ::apb::api::gift::ReceiveGiftResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GiftService::ReceiveGift", controller, request, response, done, &GiftService::ReceiveGiftImpl);
}


void GiftService::GetGiftList(::google::protobuf::RpcController* controller,
                    const ::apb::api::gift::GetGiftListRequest* request,
                    ::apb::api::gift::GetGiftListResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GiftService::GetGiftList", controller, request, response, done, &GiftService::GetGiftListImpl);
}

void GiftService::GetGiftReceiveHistoryList(::google::protobuf::RpcController* controller,
                    const ::google::protobuf::Empty* request,
                    ::apb::api::gift::GetGiftReceiveHistoryListResponse* response,
                    ::google::protobuf::Closure* done) {

    return inChangesetCall("GiftService::GetGiftReceiveHistoryList", controller, request, response, done, &GiftService::GetGiftReceiveHistoryListImpl);
}

void GiftService::ReceiveGiftImpl(UserContext &user,
                        const ::apb::api::gift::ReceiveGiftRequest* request,
                    ::apb::api::gift::ReceiveGiftResponse* response) {

    user.purgeStaleGifts();

    throw std::runtime_error("not implemented");
}

std::unique_ptr<sqlite::Statement> GiftService::runGiftListQuery(UserContext &user, const ::apb::api::gift::GetGiftListRequest* request, bool isTotalCount) {
    std::stringstream queryText;
    queryText << "SELECT ";

    if(isTotalCount) {
        queryText << "COUNT(gift_id)";
    } else {
        queryText << "gift_id, expires_datetime, gift_data, grant_datetime";
    }

    queryText << " FROM internal_user_gift WHERE user_id = ? AND received_datetime = 0";

    bool hasKindCriteria = false;

    for(auto type: request->reward_kind_type()) {
        if(type == static_cast<int32_t>(GiftRewardKindFilterType::NONE)) {
            hasKindCriteria = false;
            break;
        } else {
            hasKindCriteria = true;
        }
    }

    if(hasKindCriteria) {
        queryText << " AND reward_kind_type IS IN (";
        bool first = true;

        for(auto type: request->reward_kind_type()) {
            if(first) {
                first = false;
            } else {
                queryText << ", ";
            }

            queryText << type;
        }

        queryText << ")";
    }

    auto expirationFilter = static_cast<GiftExpirationFilterType>(request->expiration_type());
    if(expirationFilter == GiftExpirationFilterType::ONLY_EXPIRE) {
        queryText << " AND expires_datetime > 0";
    } else if(expirationFilter == GiftExpirationFilterType::ONLY_NOT_EXPIRE) {
        queryText << " AND expires_datetime = 0";
    }

    if(!isTotalCount) {

        if(request->previous_cursor() != 0) {
            queryText << " AND gift_id < ?";
        }

        if(request->next_cursor() != 0) {
            queryText << " AND gift_id > ?";
        }

        queryText << " ORDER BY gift_id";

        if(request->is_ascending_sort()) {
            queryText << " ASC";
        }

        queryText << " LIMIT ?";
    }



    auto query = user.db().prepare(queryText.view());
    int parameterIndex = 0;
    query->bind(++parameterIndex, user.userId());

    if(request->previous_cursor() != 0) {
        query->bind(++parameterIndex, request->previous_cursor());
    }

    if(request->next_cursor() != 0) {
        query->bind(++parameterIndex, request->next_cursor());
    }

    if(!isTotalCount) {
        query->bind(++parameterIndex, request->get_count());
    }

    return query;

}

void GiftService::GetGiftListImpl(UserContext &user,
                     const ::apb::api::gift::GetGiftListRequest* request,
                     ::apb::api::gift::GetGiftListResponse* response) {

    user.purgeStaleGifts();

    auto countQuery = runGiftListQuery(user, request, true);
    int32_t count = 0;
    if(countQuery->step()) {
        count = countQuery->columnInt(0);
    }
    countQuery->reset();

    auto perPage = std::max<int32_t>(request->get_count(), 1);;
    auto pages = count / perPage;
    if((count % perPage) != 0) {
        pages++;
    }
    response->set_total_page_count(pages);

    auto query = runGiftListQuery(user, request, false);

    while(query->step()) {
        auto id = query->columnInt(0);

        if(response->previous_cursor() == 0) {
            response->set_previous_cursor(id);
        }

        response->set_next_cursor(id);

        auto gift = response->add_gift();

        auto expirationDatetime = query->columnInt64(1);

        gift->set_user_gift_uuid(std::to_string(id));

        if(expirationDatetime != 0) {
            setTimestampFromNetTimestamp(gift->mutable_expiration_datetime(), expirationDatetime);
        }

        auto giftDataBytes = query->columnBlob(2);
        auto giftDataLength = query->columnBytes(2);

        auto giftData = gift->mutable_gift_common();
        giftData->ParseFromArray(giftDataBytes, giftDataLength);
        setTimestampFromNetTimestamp(giftData->mutable_grant_datetime(), query->columnInt64(3));

    }
}

void GiftService::setTimestampFromNetTimestamp(google::protobuf::Timestamp *datetime, int64_t timestamp) {

    datetime->set_seconds(timestamp / 1000);
    datetime->set_nanos((timestamp % 1000) * 1000000);
}

void GiftService::GetGiftReceiveHistoryListImpl(UserContext &user,
                                    const ::google::protobuf::Empty* request,
                                    ::apb::api::gift::GetGiftReceiveHistoryListResponse* response) {

    user.purgeStaleGifts();

    auto query = user.db().prepare(R"SQL(
        SELECT received_datetime, gift_data, grant_datetime
        FROM internal_user_gift
        WHERE user_id = ? AND received_datetime != 0
        ORDER BY gift_id DESC
        LIMIT 100
    )SQL");
    query->bind(1, user.userId());

    while(query->step()) {

        auto gift = response->add_gift();

        setTimestampFromNetTimestamp(gift->mutable_received_datetime(), query->columnInt64(0));

        auto giftDataBytes = query->columnBlob(1);
        auto giftDataLength = query->columnBytes(1);

        auto giftData = gift->mutable_giftcommon();
        giftData->ParseFromArray(giftDataBytes, giftDataLength);
        setTimestampFromNetTimestamp(giftData->mutable_grant_datetime(), query->columnInt64(2));
    }
}

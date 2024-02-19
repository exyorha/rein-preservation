#ifndef SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H

#include <grpcpp/server_context.h>

#include <cstdio>

#include <DataModel/Database.h>
#include <DataModel/ChangesetCapturingScope.h>

#include <DataModel/Sqlite/Transaction.h>

#include <service/DeckService.pb.h>
#include <service/QuestService.pb.h>

#include <functional>

class Database;

namespace sqlite {
    class Database;
}

class CommonService {
protected:
    explicit CommonService(Database &db);
    ~CommonService();

    CommonService(const CommonService &other) = delete;
    CommonService &operator =(const CommonService &other) = delete;

    inline sqlite::Database &db() const {
        return m_db.db();
    }

    inline Database &dataModel() const {
        return m_db;
    }

    template<typename RequestType, typename ResponseType, typename OuterClass>
    ::grpc::Status inCall(
        const char *callName,
        ::grpc::ServerContext* context,
        const RequestType *request,
        ResponseType *response,
        void (OuterClass::*handler)(const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, [this, callName, handler, request, response, context]() {
            addDateToContext(context);

            std::unique_lock<std::mutex> locker(m_db.callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            {
                sqlite::Transaction callTransaction(&db());

                (this->*static_cast<void(CommonService::*)(const RequestType *request, ResponseType *response)>(handler))(request, response);

                callTransaction.commit();
            }

            fputs("Finished with RPC call:\n", stdout);
            fputs(response->DebugString().c_str(), stdout);
        });
    }

    template<typename RequestType, typename ResponseType, typename OuterClass>
    ::grpc::Status inAuthenticatedCall(
        const char *callName,
        ::grpc::ServerContext* context,
        const RequestType *request,
        ResponseType *response,
        void (OuterClass::*handler)(int64_t userId, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, [this, callName, context, handler, request, response]() {
            addDateToContext(context);

            std::unique_lock<std::mutex> locker(m_db.callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            {
                sqlite::Transaction callTransaction(&db());

                auto userId = authenticate(context);

                (this->*static_cast<void(CommonService::*)(int64_t userId, const RequestType *request, ResponseType *response)>(handler))(
                    userId, request, response
                );

                callTransaction.commit();
            }

            fputs("Finished with RPC call:\n", stdout);
            fputs(response->DebugString().c_str(), stdout);
        });
    }

    template<typename RequestType, typename ResponseType, typename OuterClass>
    ::grpc::Status inChangesetCall(
        const char *callName,
        ::grpc::ServerContext* context,
        const RequestType *request,
        ResponseType *response,
        void (OuterClass::*handler)(int64_t userId, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, [this, callName, context, handler, request, response]() {
            addDateToContext(context);

            std::unique_lock<std::mutex> locker(m_db.callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            ChangesetCapturingScope changesetScope(m_db.db());

            {
                sqlite::Transaction callTransaction(&db());

                auto userId = authenticate(context);

                (this->*static_cast<void(CommonService::*)(int64_t userId, const RequestType *request, ResponseType *response)>(handler))(
                    userId, request, response
                );

                callTransaction.commit();
            }

            changesetScope.serialize(response->mutable_diff_user_data());

            fputs("Finished with RPC call:\n", stdout);
            fputs(response->DebugString().c_str(), stdout);
        });
    }

    void buildDefaultDeckIfNoneExists(int64_t userId);

protected:
    struct DeckInDatabaseRepresentation {
        std::array<std::string, 3> characterUUIDs;
    };

    void replaceDeck(int64_t userId, int32_t deckType, int32_t userDeckNumber, const apb::api::deck::Deck *deckDefinition);

    void replaceDeckCharacters(int64_t userId,
                               DeckInDatabaseRepresentation &storedDeck,
                               const ::apb::api::deck::Deck *deck);

    void replaceDeckCharacter(int64_t userId,
                              std::string &characterUUID,
                              const ::apb::api::deck::DeckCharacter *character);


    void givePossession(int64_t userId, int32_t possessionType, int32_t possessionId, int32_t count,
                        google::protobuf::RepeatedPtrField<apb::api::quest::QuestReward> *addToQuestRewards = nullptr);

    void giveUserExperience(int64_t userId, int32_t experience);

    void giveUserDeckExperience(
        int64_t userId,
        int32_t deckType,
        int32_t userDeckNumber,
        int32_t characterExperience,
        int32_t costumeExperience);

    void giveUserDeckCharacterExperience(
        int64_t userId,
        const std::string &userDeckCharacterUuid,
        int32_t characterExperience,
        int32_t costumeExperience);

    void giveUserCostumeExperience(int64_t userId, const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserWeaponExperience(int64_t userId, const std::string &userCostumeUuid, int32_t characterExperience, int32_t costumeExperience);
    void giveUserCharacterExperience(int64_t userId, int32_t characterId, int32_t characterExperience);

    int32_t getIntConfig(const std::string_view &setting) const;

    inline int32_t consumableItemIdForGold() const {
        return getIntConfig("CONSUMABLE_ITEM_ID_FOR_GOLD");
    }

    inline int32_t userLevelExpNumericalParameterMapID() const {
        return getIntConfig("USER_LEVEL_EXP_NUMERICAL_PARAMETER_MAP_ID");
    }

    std::optional<int32_t> evaluateNumericalParameterMap(int32_t mapId, int32_t value);

private:
    ::grpc::Status guardedCall(const char *name, const std::function<void()> &body);

    void addDateToContext(::grpc::ServerContext* context);

    /*
     * Returns user ID or throws an exception.
     */
    int64_t authenticate(::grpc::ServerContext *context);

    Database &m_db;
};

#endif

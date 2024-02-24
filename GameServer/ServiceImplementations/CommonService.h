#ifndef SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H

#include <grpcpp/server_context.h>

#include <cstdio>

#include <DataModel/Database.h>
#include <DataModel/ChangesetCapturingScope.h>
#include <DataModel/UserContext.h>

#include <DataModel/Sqlite/Transaction.h>

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

    [[deprecated]]
    inline sqlite::Database &db() const {
        return m_db.db();
    }

    [[deprecated]]
    inline Database &dataModel() const {
        return m_db.dataModel();
    }

    inline DatabaseContext &global() {
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

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            {
                sqlite::Transaction callTransaction(&m_db.db());

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
        void (OuterClass::*handler)(UserContext &context, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, [this, callName, context, handler, request, response]() {
            addDateToContext(context);

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            {
                sqlite::Transaction callTransaction(&m_db.db());

                auto userId = authenticate(context);

                UserContext ctx(m_db, userId);

                (this->*static_cast<void(CommonService::*)(UserContext &ctx, const RequestType *request, ResponseType *response)>(handler))(
                    ctx, request, response
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
        void (OuterClass::*handler)(UserContext &user, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, [this, callName, context, handler, request, response]() {
            addDateToContext(context);

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            printf("Processing RPC call: %s\n", callName);
            fputs(request->DebugString().c_str(), stdout);

            ChangesetCapturingScope changesetScope(m_db.db());

            {
                sqlite::Transaction callTransaction(&m_db.db());

                auto userId = authenticate(context);

                UserContext ctx(m_db, userId);

                (this->*static_cast<void(CommonService::*)(UserContext &ctx, const RequestType *request, ResponseType *response)>(handler))(
                    ctx, request, response
                );

                callTransaction.commit();
            }

            changesetScope.serialize(response->mutable_diff_user_data());

            fputs("Finished with RPC call:\n", stdout);
            fputs(response->DebugString().c_str(), stdout);
        });
    }

private:
    ::grpc::Status guardedCall(const char *name, const std::function<void()> &body);

    void addDateToContext(::grpc::ServerContext* context);

    /*
     * Returns user ID or throws an exception.
     */
    int64_t authenticate(::grpc::ServerContext *context);

    DatabaseContext m_db;
};

#endif

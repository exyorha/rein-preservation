#ifndef SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H
#define SERVICE_IMPLEMENTATIONS_COMMON_SERVICE_H

#include <google/protobuf/service.h>

#include <cstdio>

#include <DataModel/Database.h>
#include <DataModel/ChangesetCapturingScope.h>
#include <DataModel/UserContext.h>

#include <DataModel/Sqlite/Transaction.h>

#include <LLServices/Logging/LogFacility.h>

#include <functional>

class Database;

namespace sqlite {
    class Database;
}

extern LLServices::LogFacility LogRPC;

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
    void inCall(
        const char *callName,
        ::google::protobuf::RpcController* controller,
        const RequestType *request,
        ResponseType *response,
        ::google::protobuf::Closure* done,
        void (OuterClass::*handler)(const RequestType *request, ResponseType *response)) {

        guardedCall(callName, controller, done, [this, callName, handler, request, response, controller]() {
            addDateToContext(controller);

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            LogRPC.debug("Processing RPC call: %s\n%s", callName, request->DebugString().c_str());

            {
                sqlite::Transaction callTransaction(&m_db.db());

                (this->*static_cast<void(CommonService::*)(const RequestType *request, ResponseType *response)>(handler))(request, response);

                callTransaction.commit();
            }

            LogRPC.debug("Finished with RPC call:\n%s", response->DebugString().c_str());
        });
    }

    template<typename RequestType, typename ResponseType, typename OuterClass>
    void inAuthenticatedCall(
        const char *callName,
        ::google::protobuf::RpcController* controller,
        const RequestType *request,
        ResponseType *response,
        ::google::protobuf::Closure* done,
        void (OuterClass::*handler)(UserContext &context, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, controller, done, [this, callName, controller, handler, request, response]() {
            addDateToContext(controller);

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            LogRPC.debug("Processing RPC call: %s\n%s", callName, request->DebugString().c_str());

            {
                sqlite::Transaction callTransaction(&m_db.db());

                auto userId = authenticate(controller);

                UserContext ctx(m_db, userId);

                (this->*static_cast<void(CommonService::*)(UserContext &ctx, const RequestType *request, ResponseType *response)>(handler))(
                    ctx, request, response
                );

                callTransaction.commit();
            }

            LogRPC.debug("Finished with RPC call:\n%s", response->DebugString().c_str());
        });
    }

    template<typename RequestType, typename ResponseType, typename OuterClass>
    void inChangesetCall(
        const char *callName,
        ::google::protobuf::RpcController* controller,
        const RequestType *request,
        ResponseType *response,
        ::google::protobuf::Closure* done,
        void (OuterClass::*handler)(UserContext &user, const RequestType *request, ResponseType *response)) {

        return guardedCall(callName, controller, done, [this, callName, controller, handler, request, response]() {
            addDateToContext(controller);

            std::unique_lock<std::mutex> locker(m_db.dataModel().callMutex);

            LogRPC.debug("Processing RPC call: %s\n%s", callName, request->DebugString().c_str());

            ChangesetCapturingScope changesetScope(m_db.db());

            {
                sqlite::Transaction callTransaction(&m_db.db());

                auto userId = authenticate(controller);

                UserContext ctx(m_db, userId);

                (this->*static_cast<void(CommonService::*)(UserContext &ctx, const RequestType *request, ResponseType *response)>(handler))(
                    ctx, request, response
                );

                callTransaction.commit();
            }

            changesetScope.serialize(response->mutable_diff_user_data());

            LogRPC.debug("Finished with RPC call:\n%s", response->DebugString().c_str());
        });
    }

private:
    void guardedCall(const char *name,
        ::google::protobuf::RpcController* controller,
        ::google::protobuf::Closure* done,
        const std::function<void()> &body);

    void addDateToContext(::google::protobuf::RpcController* controller);

    /*
     * Returns user ID or throws an exception.
     */
    int64_t authenticate(::google::protobuf::RpcController* controller);

    DatabaseContext m_db;
};

#endif

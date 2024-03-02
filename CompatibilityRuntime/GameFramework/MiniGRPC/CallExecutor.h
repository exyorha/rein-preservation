#ifndef MINI_GRPC_CALL_EXECUTOR_H
#define MINI_GRPC_CALL_EXECUTOR_H

#include <memory>
#include <mutex>
#include <list>

namespace minigrpc {

    class ExecutingCall;

    class CallExecutor {
    public:
        CallExecutor();
        ~CallExecutor();

        CallExecutor(const CallExecutor &other) = delete;
        CallExecutor &operator =(const CallExecutor &other) = delete;

        void update();

        void add(std::unique_ptr<ExecutingCall> &&call);

    private:
        std::mutex m_queueMutex;
        std::list<std::unique_ptr<ExecutingCall>> m_calls;
    };
};

#endif

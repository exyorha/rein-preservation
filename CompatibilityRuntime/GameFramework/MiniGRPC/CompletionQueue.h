#ifndef COMPLETION_QUEUE_H
#define COMPLETION_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>

struct grpc_event;

namespace minigrpc {

    class CompletionQueue {
    public:
        explicit CompletionQueue(bool asynchronous);
        ~CompletionQueue();

        CompletionQueue(const CompletionQueue &other) = delete;
        CompletionQueue &operator =(const CompletionQueue &other) = delete;

        void shutdown();

        grpc_event next();

        void post(void *tag, int success);

        static void setGlobalShutdown();

    private:
        struct EnqueuedEvent {
            void *tag;
            int success;
        };

        static std::mutex m_queueMutex;
        static std::condition_variable m_queueCondvar;
        bool m_shutdown;
        static bool m_globalShutdown;
        std::deque<EnqueuedEvent> m_events;
    };

}

#endif

#include "CompletionQueue.h"
#include "grpcsharp.h"

#include <cstdio>
#include <stdexcept>

namespace minigrpc {

    CompletionQueue::CompletionQueue(bool asynchronous) : m_shutdown(false) {
        printf("CompletionQueue: created new %p, asynchronous: %d\n", this, asynchronous);
    }

    CompletionQueue::~CompletionQueue() {
        printf("CompletionQueue: destroyed %p\n", this);
    }

    void CompletionQueue::shutdown() {
        printf("CompletionQueue: shutdown %p\n", this);

        {
            std::unique_lock<std::mutex> locker(m_queueMutex);
            m_shutdown = true;
        }
        m_queueCondvar.notify_all();
    }

    grpc_event CompletionQueue::next() {
        std::unique_lock<std::mutex> locker(m_queueMutex);

        m_queueCondvar.wait(locker, [this]() -> bool {
            return m_shutdown || !m_events.empty();
        });

        if(!m_events.empty()) {
            const auto &postedEvent = m_events.front();

            grpc_event event = {
                .type = GRPC_OP_COMPLETE,
                .success = postedEvent.success,
                .tag = postedEvent.tag
            };

            m_events.pop_front();

            return event;
        } else if(m_shutdown) {
            grpc_event shutdownEvent = {
                .type = GRPC_QUEUE_SHUTDOWN,
                .success = 0
            };
            return shutdownEvent;
        } else {
            throw std::logic_error("CompletionQueue::next: unexpected exit");
        }
    }

    void CompletionQueue::post(void *tag, int success) {

        {

            std::unique_lock<std::mutex> locker(m_queueMutex);
            m_events.emplace_back(EnqueuedEvent{
                .tag = tag,
                .success = success
            });
        }

        m_queueCondvar.notify_one();
    }
}

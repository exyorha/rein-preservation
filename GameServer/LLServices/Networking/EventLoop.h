#ifndef LLSERVICES_NETWORKING_EVENT_LOOP_H
#define LLSERVICES_NETWORKING_EVENT_LOOP_H

#include <atomic>
#include <deque>
#include <optional>

#include <LLServices/Networking/EventTypes.h>
#include <LLServices/Networking/BeforeWaitNotifier.h>
#include <LLServices/Networking/BeforeWaitListener.h>

namespace LLServices {
    class DeferredDeletable;

    class EventLoop final : private BeforeWaitListener {
    public:
        EventLoop();
        ~EventLoop();

        EventLoop(const EventLoop &other) = delete;
        EventLoop &operator =(const EventLoop &other) = delete;

        inline struct event_base *base() const {
            return m_base.get();
        }

        void run(bool once = false);

        void stop();
        void clearStop();

        void deferredDelete(DeferredDeletable *deletable);

    private:
        static void logCallback(int severity, const char *msg);
        void beforeWait(BeforeWaitNotifier *notifier) override;

        class GlobalInitReferencer {
        public:
            GlobalInitReferencer();
            ~GlobalInitReferencer();

            GlobalInitReferencer(const GlobalInitReferencer &other) = delete;
            GlobalInitReferencer &operator =(const GlobalInitReferencer &other) = delete;

        private:
            static std::atomic_uint m_uses;
        };

        GlobalInitReferencer m_uses;
        EventBasePtr m_base;
        std::deque<DeferredDeletable *> m_deleteQueue;
        std::optional<BeforeWaitNotifier> m_beforeWait;
        std::atomic_bool m_stop;
    };
}

#endif

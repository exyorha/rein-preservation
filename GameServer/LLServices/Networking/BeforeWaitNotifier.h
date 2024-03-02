#ifndef LLSERVICES_NETWORKING_BEFORE_WAIT_NOTIFIER_H
#define LLSERVICES_NETWORKING_BEFORE_WAIT_NOTIFIER_H

#include <LLServices/Networking/EventTypes.h>

namespace LLServices {
    class EventLoop;
    class BeforeWaitListener;

    class BeforeWaitNotifier {
    public:
        BeforeWaitNotifier(EventLoop *eventLoop, BeforeWaitListener *listener);
        ~BeforeWaitNotifier();

        BeforeWaitNotifier(const BeforeWaitNotifier &other) = delete;
        BeforeWaitNotifier &operator =(const BeforeWaitNotifier &other) = delete;

    private:
        static void callback(struct evwatch *watch, const struct evwatch_prepare_cb_info *info, void *data) noexcept;

        BeforeWaitListener *m_listener;
        WatchPtr m_watch;
    };
}

#endif

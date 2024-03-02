#ifndef LLSERVICES_NETWORKING_AFTER_WAIT_NOTIFIER_H
#define LLSERVICES_NETWORKING_AFTER_WAIT_NOTIFIER_H

#include <LLServices/Networking/EventTypes.h>

namespace LLServices {
    class EventLoop;
    class AfterWaitListener;

    class AfterWaitNotifier {
    public:
        AfterWaitNotifier(EventLoop *eventLoop, AfterWaitListener *listener);
        ~AfterWaitNotifier();

        AfterWaitNotifier(const AfterWaitNotifier &other) = delete;
        AfterWaitNotifier &operator =(const AfterWaitNotifier &other) = delete;

    private:
        static void callback(struct evwatch *watch, const struct evwatch_check_cb_info *info, void *data) noexcept;

        AfterWaitListener *m_listener;
        WatchPtr m_watch;
    };
}

#endif

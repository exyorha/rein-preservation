#ifndef LLSERVICES_NETWORKING_AFTER_WAIT_LISTENER_H
#define LLSERVICES_NETWORKING_AFTER_WAIT_LISTENER_H

namespace LLServices {
    class AfterWaitNotifier;

    class AfterWaitListener {
    protected:
        AfterWaitListener();
        ~AfterWaitListener();

    public:
        AfterWaitListener(const AfterWaitListener &other) = delete;
        AfterWaitListener &operator =(const AfterWaitListener &other) = delete;

        virtual void afterWait(AfterWaitNotifier *notifier) = 0;
    };
}

#endif

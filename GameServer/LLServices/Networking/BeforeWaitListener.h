#ifndef LLSERVICES_NETWORKING_BEFORE_WAIT_LISTENER_H
#define LLSERVICES_NETWORKING_BEFORE_WAIT_LISTENER_H

namespace LLServices {
    class BeforeWaitNotifier;

    class BeforeWaitListener {
    protected:
        BeforeWaitListener();
        ~BeforeWaitListener();

    public:
        BeforeWaitListener(const BeforeWaitListener &other) = delete;
        BeforeWaitListener &operator =(const BeforeWaitListener &other) = delete;

        virtual void beforeWait(BeforeWaitNotifier *notifier) = 0;
    };
}

#endif

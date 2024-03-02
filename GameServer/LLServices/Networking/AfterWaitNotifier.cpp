#include <LLServices/Networking/AfterWaitNotifier.h>
#include <LLServices/Networking/AfterWaitListener.h>

#include <LLServices/Networking/EventLoop.h>

namespace LLServices {

    AfterWaitNotifier::AfterWaitNotifier(EventLoop *eventLoop, AfterWaitListener *listener) : m_listener(listener) {
        auto rawWatch = evwatch_check_new(eventLoop->base(), callback, this);
        if(!rawWatch)
            throw std::bad_alloc();

        m_watch.reset(rawWatch);
    }

    AfterWaitNotifier::~AfterWaitNotifier() = default;

    void AfterWaitNotifier::callback(struct evwatch *watch, const struct evwatch_check_cb_info *info, void *data) noexcept {
        (void)watch;
        (void)info;

        auto this_ = static_cast<AfterWaitNotifier *>(data);

        this_->m_listener->afterWait(this_);
    }
}

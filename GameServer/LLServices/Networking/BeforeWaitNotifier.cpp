#include <LLServices/Networking/BeforeWaitNotifier.h>
#include <LLServices/Networking/BeforeWaitListener.h>

#include <LLServices/Networking/EventLoop.h>

namespace LLServices {

    BeforeWaitNotifier::BeforeWaitNotifier(EventLoop *eventLoop, BeforeWaitListener *listener) : m_listener(listener) {
        auto rawWatch = evwatch_prepare_new(eventLoop->base(), callback, this);
        if(!rawWatch)
            throw std::bad_alloc();

        m_watch.reset(rawWatch);
    }

    BeforeWaitNotifier::~BeforeWaitNotifier() = default;

    void BeforeWaitNotifier::callback(struct evwatch *watch, const struct evwatch_prepare_cb_info *info, void *data) noexcept {
        (void)watch;
        (void)info;

        auto this_ = static_cast<BeforeWaitNotifier *>(data);

        this_->m_listener->beforeWait(this_);
    }
}

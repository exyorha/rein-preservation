#include <LLServices/Networking/EventLoop.h>
#include <LLServices/Logging/LogFacility.h>
#include <LLServices/Networking/DeferredDeletable.h>

#include <event2/thread.h>
#include <signal.h>

#include <new>

namespace LLServices {
    LogFacility LogLibevent("libevent");

    std::atomic_uint EventLoop::GlobalInitReferencer::m_uses;

    EventLoop::EventLoop() : m_stop(false) {
        auto rawConfig = event_config_new();
        if(!rawConfig)
            throw std::bad_alloc();

        EventConfigPtr config(rawConfig);

        auto rawBase = event_base_new_with_config(config.get());
        if(!rawBase)
            throw std::bad_alloc();

        m_base.reset(rawBase);

        evthread_make_base_notifiable(m_base.get());

        m_beforeWait.emplace(this, static_cast<BeforeWaitListener *>(this));
    }

    EventLoop::~EventLoop() = default;

    void EventLoop::stop() {
        m_stop = true;
        event_base_loopbreak(m_base.get());
    }

    void EventLoop::clearStop() {
        m_stop = false;
        event_base_loopcontinue(m_base.get());
    }

    EventLoop::GlobalInitReferencer::GlobalInitReferencer() {
        if(m_uses.fetch_add(1) == 0) {
            event_set_log_callback(logCallback);
            //event_enable_debug_logging(EVENT_DBG_ALL);
        }
    }

    EventLoop::GlobalInitReferencer::~GlobalInitReferencer() {
        if(m_uses.fetch_sub(1) == 1) {
            libevent_global_shutdown();
            event_set_log_callback(nullptr);
        }
    }

    void EventLoop::logCallback(int severity, const char *msg) {
        LogLevel level;

        switch(severity) {
            case EVENT_LOG_DEBUG:
                level = LogLevel::Debug;
                break;

            case EVENT_LOG_MSG:
                level = LogLevel::Info;
                break;

            case EVENT_LOG_WARN:
                level = LogLevel::Warning;
                break;

            default:
            case EVENT_LOG_ERR:
                level = LogLevel::Error;
                break;
        }

        LogLibevent.message(level, "%s", msg);

    }

    void EventLoop::run(bool once) {
        if(m_stop)
            return;

        int flags = 0;

        if(once)
            flags |= EVLOOP_ONCE;

        if(event_base_loop(m_base.get(), flags) < 0)
            throw std::runtime_error("event_base_loop failed");
    }

    void EventLoop::beforeWait(BeforeWaitNotifier *notifier) {
        (void)notifier;

        while(!m_deleteQueue.empty()) {
            auto object = m_deleteQueue.front();
            m_deleteQueue.pop_front();

            object->deferredDelete();
        }
    }

    void EventLoop::deferredDelete(DeferredDeletable *deletable) {
        m_deleteQueue.push_back(deletable);
    }
}

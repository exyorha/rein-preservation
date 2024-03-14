#include <LLServices/Networking/FileDescriptorWatcher.h>
#include <LLServices/Networking/FileDescriptorListener.h>
#include <LLServices/Networking/EventLoop.h>
#include <LLServices/Networking/FileDescriptor.h>

namespace LLServices {
    FileDescriptorWatcher::FileDescriptorWatcher(
        EventLoop *eventLoop,
        const FileDescriptor &fd,
        FileDescriptorListener *listener,
        unsigned int events) : m_listener(listener) {

        auto rawEvent = event_new(eventLoop->base(), fd, events | EV_PERSIST, callback, this);
        if(!rawEvent)
            throw std::bad_alloc();

        m_event.reset(rawEvent);

        if(events != 0) {
            if(event_add(m_event.get(), nullptr) < 0)
                throw std::logic_error("event_add failed");
        }
    }

    FileDescriptorWatcher::~FileDescriptorWatcher() = default;

    void FileDescriptorWatcher::setEventsToWatch(unsigned int events) {
        event_del(m_event.get());

        auto base = event_get_base(m_event.get());
        auto fd = event_get_fd(m_event.get());

        if(event_assign(m_event.get(),
                     base,
                     fd,
                     events | EV_PERSIST,
                     callback,
                     this) < 0)
            throw std::logic_error("event_assign failed");

        if(events != 0) {
            if(event_add(m_event.get(), nullptr) < 0)
                throw std::logic_error("event_add failed");
        }
    }

    void FileDescriptorWatcher::callback(evutil_socket_t fd, short events, void *arg) noexcept {
        (void)fd;

        auto this_ = static_cast<FileDescriptorWatcher *>(arg);

        this_->m_listener->fileDescriptorEvent(this_, events);
    }
}

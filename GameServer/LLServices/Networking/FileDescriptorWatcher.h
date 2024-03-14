#ifndef LLSERVICES_NETWORKING_FILE_DESCRIPTOR_WATCHER_H
#define LLSERVICES_NETWORKING_FILE_DESCRIPTOR_WATCHER_H

#include <LLServices/Networking/EventTypes.h>

namespace LLServices {

    class EventLoop;
    class FileDescriptor;
    class FileDescriptorListener;

    class FileDescriptorWatcher {
    public:
        explicit FileDescriptorWatcher(EventLoop *eventLoop, const FileDescriptor &fd, FileDescriptorListener *listener, unsigned int events = 0);
        ~FileDescriptorWatcher();

        FileDescriptorWatcher(const FileDescriptorWatcher &other) = delete;
        FileDescriptorWatcher &operator =(const FileDescriptorWatcher &other) = delete;

        void setEventsToWatch(unsigned int events);

    private:
        static void callback(evutil_socket_t fd, short events, void *arg) noexcept;

        FileDescriptorListener *m_listener;
        EventPtr m_event;
    };

}

#endif


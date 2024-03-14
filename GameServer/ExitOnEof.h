#ifndef EXIT_ON_EOF_H
#define EXIT_ON_EOF_H

#include <LLServices/Networking/EventTypes.h>
#include <LLServices/Networking/FileDescriptor.h>
#include <LLServices/Networking/FileDescriptorListener.h>
#include <LLServices/Networking/FileDescriptorWatcher.h>

namespace LLServices {
    class EventLoop;
}

class ExitOnEof final : private LLServices::FileDescriptorListener {
public:
    ExitOnEof(LLServices::EventLoop *eventLoop, evutil_socket_t fd);
    ~ExitOnEof();

    ExitOnEof(const ExitOnEof &other) = delete;
    ExitOnEof &operator =(const ExitOnEof &other) = delete;

private:
    void fileDescriptorEvent(LLServices::FileDescriptorWatcher *watcher, uint32_t events) override;

    LLServices::EventLoop *m_eventLoop;
    LLServices::FileDescriptor m_fd;
    LLServices::FileDescriptorWatcher m_watcher;

};

#endif

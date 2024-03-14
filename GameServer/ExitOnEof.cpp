#include "ExitOnEof.h"

#include <LLServices/Networking/EventLoop.h>

ExitOnEof::ExitOnEof(LLServices::EventLoop *eventLoop, evutil_socket_t fd) : m_eventLoop(eventLoop), m_fd(fd),
    m_watcher(m_eventLoop, m_fd, this, EV_READ) {

}


ExitOnEof::~ExitOnEof() = default;

void ExitOnEof::fileDescriptorEvent(LLServices::FileDescriptorWatcher *watcher, uint32_t events) {
    if(events & EV_READ) {
        while(true) {
            char dump[128];
    #if defined(_WIN32)
            auto result = recv(m_fd, dump, sizeof(dump), 0);
    #else
            auto result = read(m_fd, dump, sizeof(dump));
    #endif
            if(result < 0) {
                auto error = evutil_socket_geterror(m_fd);

    #if defined(_WIN32)
                if(error == WSAEWOULDBLOCK)
                    return;
    #else
                if(error == EWOULDBLOCK)
                    return;
    #endif

                throw std::runtime_error("an error occurred while reading from the exit pipe: " + std::to_string(error));
            }

            if(result == 0) {
                m_watcher.setEventsToWatch(0);
                m_eventLoop->stop();
                return;
            }
        }
    }
}

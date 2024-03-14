#ifndef LLSERVICES_NETWORKING_FILE_DESCRIPTOR_LISTENER_H
#define LLSERVICES_NETWORKING_FILE_DESCRIPTOR_LISTENER_H

#include <cstdint>

namespace LLServices {
    class FileDescriptorWatcher;

    class FileDescriptorListener {
    protected:
        FileDescriptorListener();
        ~FileDescriptorListener();

    public:
        FileDescriptorListener(const FileDescriptorListener &other) = delete;
        FileDescriptorListener &operator =(const FileDescriptorListener &other) = delete;

        virtual void fileDescriptorEvent(FileDescriptorWatcher *watcher, uint32_t events) = 0;
    };

}

#endif

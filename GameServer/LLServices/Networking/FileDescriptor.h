#ifndef LLSERVICES_NETWORKING_FILE_DESCRIPTOR_H
#define LLSERVICES_NETWORKING_FILE_DESCRIPTOR_H

#include <event2/util.h>

namespace LLServices {

    class FileDescriptor {
    public:
        FileDescriptor() noexcept;
        explicit FileDescriptor(evutil_socket_t fd) noexcept;
        ~FileDescriptor();

        FileDescriptor(const FileDescriptor &other) = delete;
        FileDescriptor &operator =(const FileDescriptor &other) = delete;

        FileDescriptor(FileDescriptor &&other) noexcept;
        FileDescriptor &operator =(FileDescriptor &&other) noexcept;

        operator evutil_socket_t() const;
        inline explicit operator bool() const {
            return m_fd >= 0;
        }

        void reset(evutil_socket_t fd = -1) noexcept;
        void swap(FileDescriptor &other) noexcept;
        evutil_socket_t release() noexcept;

    private:
        evutil_socket_t m_fd;
    };
}

#endif

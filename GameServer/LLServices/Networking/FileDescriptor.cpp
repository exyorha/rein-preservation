#include <LLServices/Networking/FileDescriptor.h>

#include <utility>
#include <stdexcept>
#include <algorithm>

namespace LLServices {

    FileDescriptor::FileDescriptor() noexcept : m_fd(-1) {

    }

    FileDescriptor::FileDescriptor(evutil_socket_t fd) noexcept : m_fd(fd) {

    }

    FileDescriptor::~FileDescriptor() {
        if(m_fd >= 0)
            evutil_closesocket(m_fd);
    }


    FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept : FileDescriptor() {
        *this = std::move(other);
    }

    FileDescriptor &FileDescriptor::operator =(FileDescriptor &&other) noexcept {
        swap(other);

        return *this;
    }

    FileDescriptor::operator evutil_socket_t() const {
        if(m_fd < 0)
            throw std::logic_error("file descriptor is not valid");

        return m_fd;
    }

    void FileDescriptor::reset(evutil_socket_t fd) noexcept {
        *this = FileDescriptor(fd);
    }

    void FileDescriptor::swap(FileDescriptor &other) noexcept {
        std::swap(m_fd, other.m_fd);
    }

    evutil_socket_t FileDescriptor::release() noexcept {
        auto result = m_fd;
        m_fd = -1;

        return result;
    }
}

#include "FileDescriptor.h"

#include <unistd.h>

#include <utility>
#include <stdexcept>
#include <algorithm>


FileDescriptor::FileDescriptor() noexcept : m_fd(-1) {

}

FileDescriptor::FileDescriptor(int fd) noexcept : m_fd(fd) {

}

FileDescriptor::~FileDescriptor() {
    if(m_fd >= 0)
        close(m_fd);
}


FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept : FileDescriptor() {
    *this = std::move(other);
}

FileDescriptor &FileDescriptor::operator =(FileDescriptor &&other) noexcept {
    swap(other);

    return *this;
}

FileDescriptor::operator int() const {
    if(m_fd < 0)
        throw std::logic_error("file descriptor is not valid");

    return m_fd;
}

void FileDescriptor::reset(int fd) noexcept {
    *this = FileDescriptor(fd);
}

void FileDescriptor::swap(FileDescriptor &other) noexcept {
    std::swap(m_fd, other.m_fd);
}

int FileDescriptor::release() noexcept {
    auto result = m_fd;
    m_fd = -1;

    return result;
}

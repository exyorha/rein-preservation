#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H


class FileDescriptor {
public:
    FileDescriptor() noexcept;
    explicit FileDescriptor(int fd) noexcept;
    ~FileDescriptor();

    FileDescriptor(const FileDescriptor &other) = delete;
    FileDescriptor &operator =(const FileDescriptor &other) = delete;

    FileDescriptor(FileDescriptor &&other) noexcept;
    FileDescriptor &operator =(FileDescriptor &&other) noexcept;

    operator int() const;
    inline explicit operator bool() const {
        return m_fd >= 0;
    }

    void reset(int fd = -1) noexcept;
    void swap(FileDescriptor &other) noexcept;
    int release() noexcept;

private:
    int m_fd;
};

#endif

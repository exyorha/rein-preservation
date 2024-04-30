#ifndef WEBVIEWHOSTCLIENTCHANNELLINUX_H
#define WEBVIEWHOSTCLIENTCHANNELLINUX_H

#include <google/protobuf/service.h>

#include <vector>

#include "WebViewSharedImageBuffer.h"

class WebViewHostClientConfiguration;

class WebViewHostClientChannelLinux final : public google::protobuf::RpcChannel {
public:
    explicit WebViewHostClientChannelLinux(const WebViewHostClientConfiguration &config);
    ~WebViewHostClientChannelLinux() override;


    void CallMethod(
        const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller,
        const google::protobuf::Message* request,
        google::protobuf::Message* response,
        google::protobuf::Closure* done) override;

    std::unique_ptr<WebViewSharedImageBuffer> allocateImageBuffer(size_t size);
    int64_t sendSharedImageBufferWithNextRequest(WebViewSharedImageBuffer *buffer);

private:
    class FileDescriptor {
    public:
        inline explicit FileDescriptor(int fd = -1) noexcept : m_fd(fd) {}
        ~FileDescriptor();

        FileDescriptor(FileDescriptor &&other) noexcept;
        FileDescriptor &operator =(FileDescriptor &&other) noexcept;

        FileDescriptor(const FileDescriptor &other) = delete;
        FileDescriptor &operator =(const FileDescriptor &other) = delete;

        inline operator int() const {
            return m_fd;
        }

    private:
        int m_fd;
    };

    class LinuxImageBuffer final : public WebViewSharedImageBuffer{
    public:
        explicit LinuxImageBuffer(size_t size);
        ~LinuxImageBuffer() override;

        inline int fd() const {
            return m_fd;
        }

        void *base() const override;
        size_t size() const override;

    private:
        FileDescriptor m_fd;
        void *m_base;
        size_t m_size;
    };

    pid_t m_pid;
    FileDescriptor m_fd;
    std::vector<unsigned char> m_receiveBuffer;
    std::vector<FileDescriptor> m_pendingFileDescriptors;

};

#endif

#ifndef WEBVIEWHOSTCLIENTCHANNELLINUX_H
#define WEBVIEWHOSTCLIENTCHANNELLINUX_H

#include <google/protobuf/service.h>

#include <vector>

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

private:
    bool ensuredWrite(const void *dest, size_t size);
    bool ensuredRead(void *dest, size_t size);

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

    pid_t m_pid;
    FileDescriptor m_fd;
    std::vector<unsigned char> m_receiveBuffer;

};

#endif

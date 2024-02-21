#ifndef WEB_VIEW_HOST_CLIENT_H
#define WEB_VIEW_HOST_CLIENT_H

#include <google/protobuf/service.h>
#include <sys/types.h>

#include <WebView/WebViewProtocol.pb.h>
#include <WebViewProtocolController.h>

class WebViewHostClient {
public:
    explicit WebViewHostClient(const std::string &hostExecutable);
    ~WebViewHostClient();

    WebViewHostClient(const WebViewHostClient &other) = delete;
    WebViewHostClient &operator =(const WebViewHostClient &other) = delete;

    inline pid_t pid() const {
        return m_pid;
    }

    inline webview::protocol::WebViewService::Stub &stub() {
        return *m_stub;
    }

    inline google::protobuf::RpcController *controller() {
        return &m_controller;
    }

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

    class Channel final : public google::protobuf::RpcChannel {
    public:
        explicit Channel(FileDescriptor &&fd);
        ~Channel() override;

        void CallMethod(
            const google::protobuf::MethodDescriptor* method,
            google::protobuf::RpcController* controller,
            const google::protobuf::Message* request,
            google::protobuf::Message* response,
            google::protobuf::Closure* done) override;

    private:
        bool ensuredWrite(const void *dest, size_t size);
        bool ensuredRead(void *dest, size_t size);

        FileDescriptor m_fd;
        std::vector<unsigned char> m_receiveBuffer;
    };

    pid_t m_pid;
    std::optional<Channel> m_channel;
    std::optional<webview::protocol::WebViewService::Stub> m_stub;
    WebViewProtocolController m_controller;
};

#endif

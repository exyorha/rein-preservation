#ifndef WEBVIEWHOSTCLIENTCHANNELWINDOWS_H
#define WEBVIEWHOSTCLIENTCHANNELWINDOWS_H

#include <google/protobuf/service.h>

#include <windows.h>

#include <vector>

class WebViewHostClientConfiguration;

class WebViewHostClientChannelWindows final : public google::protobuf::RpcChannel {
public:
    explicit WebViewHostClientChannelWindows(const WebViewHostClientConfiguration &config);
    ~WebViewHostClientChannelWindows() override;

    void CallMethod(
        const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller,
        const google::protobuf::Message* request,
        google::protobuf::Message* response,
        google::protobuf::Closure* done) override;

private:

    class HandleHolder {
    public:
        inline explicit HandleHolder(HANDLE handle = nullptr) noexcept : m_handle(handle) {}
        ~HandleHolder();

        HandleHolder(HandleHolder &&other) noexcept;
        HandleHolder &operator =(HandleHolder &&other) noexcept;

        HandleHolder(const HandleHolder &other) = delete;
        HandleHolder &operator =(const HandleHolder &other) = delete;

        inline operator HANDLE() const {
            return m_handle;
        }

        inline operator bool() const {
            return m_handle;
        }

    private:
        HANDLE m_handle;
    };

    HandleHolder m_handle;
    HandleHolder m_process;
    std::vector<unsigned char> m_receiveBuffer;
};

#endif

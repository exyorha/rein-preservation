#ifndef WEB_VIEW_HOST_CLIENT_H
#define WEB_VIEW_HOST_CLIENT_H

#include <google/protobuf/service.h>
#include <sys/types.h>
#include <filesystem>

#include <WebView/WebViewProtocol.pb.h>
#include <WebViewProtocolController.h>
#include <WebViewHostClientChannel.h>

class WebViewHostClientConfiguration;

class WebViewHostClient {
public:
    explicit WebViewHostClient(const WebViewHostClientConfiguration &configuration);
    ~WebViewHostClient();

    WebViewHostClient(const WebViewHostClient &other) = delete;
    WebViewHostClient &operator =(const WebViewHostClient &other) = delete;

    inline WebViewHostClientChannel &channel() {
        return m_channel;
    }

    inline webview::protocol::WebViewService::Stub &stub() {
        return *m_stub;
    }

    inline google::protobuf::RpcController *controller() {
        return &m_controller;
    }

private:

    WebViewHostClientChannel m_channel;
    std::optional<webview::protocol::WebViewService::Stub> m_stub;
    WebViewProtocolController m_controller;
};

#endif

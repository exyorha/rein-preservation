#ifndef WEBVIEWHOST_PROTOCOL_WEB_VIEW_PROTOCOL_CONTROLLER_H
#define WEBVIEWHOST_PROTOCOL_WEB_VIEW_PROTOCOL_CONTROLLER_H

#include <google/protobuf/service.h>
#include <optional>
#include <string>

class WebViewRPCServer;

class WebViewProtocolController final : public google::protobuf::RpcController {
public:
    explicit inline WebViewProtocolController(WebViewRPCServer *server = nullptr) : m_server(server) {}
    ~WebViewProtocolController() override = default;

    void Reset() override {
        m_error.reset();
    }

    bool Failed() const override {
        return m_error.has_value();
    }

    std::string ErrorText() const override {
        return m_error.value_or(std::string());
    }

    void StartCancel() override {

    }

    void SetFailed(const std::string& reason) override {
        m_error.emplace(reason);
    }

    bool IsCanceled() const override {
        return false;
    }

    void NotifyOnCancel(google::protobuf::Closure* callback) override {
        (void)callback;
    }

    inline WebViewRPCServer *server() const {
        return m_server;
    }

private:
    WebViewRPCServer *m_server;
    std::optional<std::string> m_error;
};

#endif

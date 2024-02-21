#ifndef WEBVIEWHOST_PROTOCOL_WEB_VIEW_PROTOCOL_CONTROLLER_H
#define WEBVIEWHOST_PROTOCOL_WEB_VIEW_PROTOCOL_CONTROLLER_H

#include <google/protobuf/service.h>
#include <optional>
#include <string>

class WebViewProtocolController final : public google::protobuf::RpcController {
public:
    WebViewProtocolController() = default;
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

private:
    std::optional<std::string> m_error;
};

#endif

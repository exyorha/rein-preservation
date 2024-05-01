#ifndef WEBVIEW_HOST_CLIENT_CONFIGURATION_H
#define WEBVIEW_HOST_CLIENT_CONFIGURATION_H

#include <filesystem>

class WebViewHostClientConfiguration {
public:
    WebViewHostClientConfiguration();
    ~WebViewHostClientConfiguration();

    WebViewHostClientConfiguration(const WebViewHostClientConfiguration &other);
    WebViewHostClientConfiguration &operator =(const WebViewHostClientConfiguration &other);

    WebViewHostClientConfiguration(WebViewHostClientConfiguration &&other) noexcept;
    WebViewHostClientConfiguration &operator =(WebViewHostClientConfiguration &&other) noexcept;

    std::filesystem::path hostExecutablePath;
    std::filesystem::path homePath;

    bool isUsable() const;

private:
    static std::filesystem::path getDefaultHostExecutablePath();
};

#endif

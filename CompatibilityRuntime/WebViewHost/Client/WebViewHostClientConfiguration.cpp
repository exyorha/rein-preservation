#include <WebViewHostClientConfiguration.h>

WebViewHostClientConfiguration::WebViewHostClientConfiguration() :
    hostExecutablePath(getDefaultHostExecutablePath()) {

}

WebViewHostClientConfiguration::~WebViewHostClientConfiguration() = default;

WebViewHostClientConfiguration::WebViewHostClientConfiguration(const WebViewHostClientConfiguration &other) = default;

WebViewHostClientConfiguration &WebViewHostClientConfiguration::operator =(const WebViewHostClientConfiguration &other) = default;

WebViewHostClientConfiguration::WebViewHostClientConfiguration(WebViewHostClientConfiguration &&other) noexcept = default;

WebViewHostClientConfiguration &WebViewHostClientConfiguration::operator =(WebViewHostClientConfiguration &&other) noexcept = default;

bool WebViewHostClientConfiguration::isUsable() const {
    return std::filesystem::exists(hostExecutablePath);
}

#ifndef _WIN32
std::filesystem::path WebViewHostClientConfiguration::getDefaultHostExecutablePath() {
    return std::filesystem::read_symlink("/proc/self/exe").parent_path() / "webview" / "linux" / "WebViewHost";
}
#endif

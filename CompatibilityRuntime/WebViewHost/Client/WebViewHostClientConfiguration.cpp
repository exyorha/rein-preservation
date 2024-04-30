#include <WebViewHostClientConfiguration.h>

#ifdef _WIN32
#include <windows.h>
#include <vector>

extern "C" {
    extern unsigned char __ImageBase;
}

#endif

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

#ifdef _WIN32
std::filesystem::path WebViewHostClientConfiguration::getDefaultHostExecutablePath() {
    std::vector<wchar_t> modulePathChars(PATH_MAX);
    DWORD outLength;
    DWORD error;

    do {
        outLength = GetModuleFileName(reinterpret_cast<HMODULE>(&__ImageBase),
                                      modulePathChars.data(), modulePathChars.size());
        if(outLength == 0) {
            throw std::runtime_error("GetModuleFileName failed");
        }

        if(error == ERROR_INSUFFICIENT_BUFFER) {
            modulePathChars.resize(modulePathChars.size() * 2);
        }

    } while(error == ERROR_INSUFFICIENT_BUFFER);

    std::filesystem::path path(modulePathChars.data(), modulePathChars.data() + outLength);

    return path.parent_path() / "webview" / "windows" / "WebViewHost.exe";
}
#else

std::filesystem::path WebViewHostClientConfiguration::getDefaultHostExecutablePath() {
    return std::filesystem::read_symlink("/proc/self/exe").parent_path() / "webview" / "linux" / "WebViewHost";
}
#endif


#ifndef WEB_CONTENT_SERVER_WEB_CONTENT_SERVER_H
#define WEB_CONTENT_SERVER_WEB_CONTENT_SERVER_H

#include "WebServices/WebRoutable.h"
#include "WebContentServer/WebContentStorage.h"

#include <filesystem>
#include <vector>
#include <optional>
#include <string_view>
#include <memory>

class WebContentServer final : public WebRoutable {
public:
    explicit WebContentServer(std::unique_ptr<WebContentStorage> &&storage);
    ~WebContentServer();

    WebContentServer(const WebContentServer &other) = delete;
    WebContentServer &operator =(const WebContentServer &other) = delete;

    void handle(const std::string_view &path, LLServices::HttpRequest &&request) override;

    void overridePath(std::string &&path, std::filesystem::path &&filename);

    inline void setFallbackPage(std::string &&fallbackPage) {
        m_fallbackPage.emplace(std::move(fallbackPage));
    }

private:

    struct MimeType {
        std::string_view extension;
        const char *contentType;
    };

    struct OverridePath {
        std::string path;
        std::filesystem::path filename;
    };

    std::optional<WebContentLocation> mapPath(const std::string_view &path);

    static std::vector<std::pair<size_t, size_t>> parseRanges(std::string_view rangeHeader, size_t fileSize);

    static std::string formatRange(const std::pair<size_t, size_t> &range, size_t fileSize);

    static size_t parseByteCount(const std::string_view &piece);

    static const MimeType m_mimeTypes[];

    bool tryServePath(const std::string_view &path, LLServices::HttpRequest &&request);

    std::unique_ptr<WebContentStorage> m_storage;
    std::vector<OverridePath> m_overridePaths;
    std::optional<std::string> m_fallbackPage;
};

#endif

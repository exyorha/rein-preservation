#ifndef WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H
#define WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H

#include <string_view>
#include <filesystem>
#include <optional>
#include <cstdint>

struct WebContentLocation {
    static constexpr uint64_t UntilEOF = UINT64_C(0xFFFFFFFFFFFFFFFF);

    std::filesystem::path containingFilePath;
    uint64_t fileOffset;
    uint64_t fileLength;
    time_t lastModified;
};

class WebContentStorage {
protected:
    WebContentStorage();

public:
    virtual ~WebContentStorage();

    WebContentStorage(const WebContentStorage &other) = delete;
    WebContentStorage &operator =(const WebContentStorage &other) = delete;

    virtual std::optional<WebContentLocation> lookup(const std::string_view &path) const = 0;
};

#endif

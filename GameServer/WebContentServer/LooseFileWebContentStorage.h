#ifndef LOOSE_FILE_WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H
#define LOOSE_FILE_WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H

#include <WebContentServer/WebContentStorage.h>

#include <filesystem>

class LooseFileWebContentStorage final : public WebContentStorage {
public:
    explicit LooseFileWebContentStorage(std::filesystem::path &&rootDirectory);
    ~LooseFileWebContentStorage() override;

    std::optional<WebContentLocation> lookup(const std::string_view &path) const override;

private:
    std::filesystem::path m_rootDirectory;
};

#endif


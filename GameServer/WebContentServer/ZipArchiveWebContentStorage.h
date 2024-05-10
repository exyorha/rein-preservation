#ifndef ZIP_ARCHIVE_WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H
#define ZIP_ARCHIVE_WEB_CONTENT_SERVER_WEB_CONTENT_STORAGE_H

#include <WebContentServer/WebContentStorage.h>
#include <ClientDataAccess/ZIPBasedStorage.h>

#include <filesystem>

class ZipArchiveWebContentStorage final : public WebContentStorage {
public:
    explicit ZipArchiveWebContentStorage(const std::filesystem::path &zipFile);
    ~ZipArchiveWebContentStorage() override;

    std::optional<WebContentLocation> lookup(const std::string_view &path) const override;

private:
    ClientDataAccess::ZIPBasedStorage m_zip;
};

#endif



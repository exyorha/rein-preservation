#include "ZipArchiveWebContentStorage.h"

ZipArchiveWebContentStorage::ZipArchiveWebContentStorage(const std::filesystem::path &zipFile) : m_zip(zipFile) {

}

ZipArchiveWebContentStorage::~ZipArchiveWebContentStorage() = default;

std::optional<WebContentLocation> ZipArchiveWebContentStorage::lookup(const std::string_view &path) const {
    auto location = m_zip.lookupLocation(path.substr(1));
    if(!location.has_value())
        return std::nullopt;


    return WebContentLocation{
        .containingFilePath = std::move(location->zipFilePath),
        .fileOffset = location->fileOffset,
        .fileLength = location->fileLength,
        .lastModified = location->modtime
    };
}

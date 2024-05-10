#include <WebContentServer/LooseFileWebContentStorage.h>

LooseFileWebContentStorage::LooseFileWebContentStorage(std::filesystem::path &&rootDirectory) : m_rootDirectory(std::move(rootDirectory)) {

}

LooseFileWebContentStorage::~LooseFileWebContentStorage() = default;

std::optional<WebContentLocation> LooseFileWebContentStorage::lookup(const std::string_view &path) const {

    auto justPath = path.substr(1);
    auto normalized = std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t *>(justPath.data()), justPath.size())).lexically_normal();

    if(normalized.empty() || !normalized.is_relative()) {
        return std::nullopt;
    }

    for(const auto &component: normalized) {
        if(component == "..") {
            return std::nullopt;
        }
    }

    return WebContentLocation{
        .containingFilePath = m_rootDirectory / normalized,
        .fileOffset = 0,
        .fileLength = WebContentLocation::UntilEOF,
        .lastModified = 0
    };
}


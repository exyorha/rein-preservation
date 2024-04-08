#include "OctoContentStorage.h"

#include <filesystem>
#include <optional>
#include <fstream>

OctoContentStorage::OctoContentStorage(const std::filesystem::path &root) : m_root(root) {

    std::optional<int> latestVersion;
    std::filesystem::path fileOfLatestVersion;

    for(const auto &listEntry : std::filesystem::directory_iterator(root / "lists")) {
        if(!listEntry.is_regular_file())
            continue;

        const auto &path = listEntry.path();

        if(!path.has_extension() || path.extension() != ".pb" || !path.has_stem())
            continue;

        int version = std::atoi(path.stem().string().c_str());

        if(!latestVersion.has_value() || *latestVersion < version) {
            latestVersion.emplace(version);
            fileOfLatestVersion = path;
        }
    }

    if(!latestVersion) {
        throw std::runtime_error("a suitable content list was not found in the content directory");
    }

    m_octoListFile = std::move(fileOfLatestVersion);

    std::optional<int64_t> latestMasterDatabaseVersion;
    std::filesystem::path latestMasterDatabaseFile;

    for(const auto &listEntry : std::filesystem::recursive_directory_iterator(root / "database")) {
        if(!listEntry.is_regular_file())
            continue;

        const auto &path = listEntry.path();
        if(path.filename() != "database.bin")
            continue;

        auto version = path.parent_path().filename().string();
        int64_t versionNumber;

        auto result = std::from_chars(version.data(), version.data() + version.size(), versionNumber);
        if(result.ec != std::errc() || result.ptr != version.data() + version.size())
            throw std::runtime_error("non-numeric master database version");

        if(!latestMasterDatabaseVersion || *latestMasterDatabaseVersion < versionNumber) {
            latestMasterDatabaseVersion = versionNumber;
            latestMasterDatabaseFile = path;
        }
    }

    if(!latestMasterDatabaseVersion)
        throw std::runtime_error("did not find a suitable master database file");

    m_masterDatabase = latestMasterDatabaseFile;

    {
        std::ifstream stream;
        stream.exceptions(std::ios::failbit | std::ios::eofbit | std::ios::badbit);
        stream.open(m_octoListFile, std::ios::in | std::ios::binary);
        stream.exceptions(std::ios::badbit);
        if(!m_database.ParseFromIstream(&stream))
            throw std::runtime_error("failed to parse the content list");
    }

    printf("Loaded the Octo content database of revision %d\n", revision());

    m_files.reserve(m_database.assetbundlelist_size() + m_database.resourcelist_size());

    for(const auto &asset: m_database.assetbundlelist()) {
        RegisteredFile registration;
        registration.entry = &asset;
        registration.isResource = false;

        auto result = m_files.emplace(asset.objectname(), registration);
        if(!result.second)
            throw std::runtime_error("duplicate asset with the object name " + asset.objectname());
    }

    for(const auto &resource : m_database.resourcelist()) {
        RegisteredFile registration;
        registration.entry = &resource;
        registration.isResource = true;

        auto result = m_files.emplace(resource.objectname(), registration);
        if(!result.second)
            throw std::runtime_error("duplicate resource with the object name " + resource.objectname());

    }
}

OctoContentStorage::~OctoContentStorage() = default;

std::optional<std::filesystem::path> OctoContentStorage::locateFile(
    const std::string &objectName,
    const std::optional<std::string_view> &md5Sum,
    const std::optional<size_t> fileSize) const {

    if(objectName.empty())
        return std::nullopt;

    bool shouldBeResource = objectName.front() == 'R';

    auto it = m_files.find(objectName.substr(1));
    if(it == m_files.end())
        return std::nullopt;

    auto data = it->second.entry;

    if(shouldBeResource != it->second.isResource) {
        printf("File kind doesn't: requested %.*s (should be resource = %d), DB has %d\n",
               static_cast<int>(md5Sum->size()), md5Sum->data(),
               shouldBeResource, it->second.isResource);
        return std::nullopt;
    }

    if(md5Sum.has_value()) {
        std::string_view bareSum(*md5Sum);

        auto delim = bareSum.find_last_of('.');
        if(delim != std::string_view::npos)
            bareSum = bareSum.substr(0, delim);

        if(md5Sum.has_value() && data->md5() != bareSum) {
            printf("MD5 doesn't match: requested %.*s, DB has %s\n",
                static_cast<int>(md5Sum->size()), md5Sum->data(),
                data->md5().c_str());
            return std::nullopt;
        }
    }

    if(fileSize.has_value() && data->size() != *fileSize) {
        printf("File size doesn't match: requested %zu, DB has %d\n",
               *fileSize, data->size());
        return std::nullopt;
    }

    if(it->second.isResource) {
        return m_root / "resources" / data->name();
    } else {
        std::string path(data->name());
        for(auto &ch: path) {
            if(ch == ')') {
                ch = '/';
            }
        }

        path.append(".unity3d");

        auto pathRelativeToRoot = std::filesystem::path("assetbundle") / std::u8string_view(reinterpret_cast<const char8_t *>(path.data()), path.size());
        auto adaptedVersion = m_root / ".." / "adapted_content" / pathRelativeToRoot;

        if(std::filesystem::exists(adaptedVersion))
            return adaptedVersion;

        return m_root / pathRelativeToRoot;
    }
}


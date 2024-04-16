#ifndef OCTO_CONTENT_STORAGE_H
#define OCTO_CONTENT_STORAGE_H

#include <filesystem>
#include <optional>

#include <Octo/Proto/Database.pb.h>

#include "ZIPBasedStorage.h"

class OctoContentStorage {
public:
    explicit OctoContentStorage(const std::filesystem::path &root);
    ~OctoContentStorage();

    OctoContentStorage(const OctoContentStorage &other) = delete;
    OctoContentStorage &operator =(const OctoContentStorage &other) = delete;

    inline int32_t revision() const {
        return m_database.revision();
    }

    std::optional<std::filesystem::path> locateFile(
        const std::string &objectName,
        const std::optional<std::string_view> &md5Sum = {},
        const std::optional<size_t> fileSize = {}) const;

    inline const std::filesystem::path &masterDatabase() const {
        return m_masterDatabase;
    }

    inline const std::filesystem::path &octoListFile() const {
        return m_octoListFile;
    }

private:
    struct RegisteredFile {
        const Octo::Proto::Data *entry;
        bool isResource;
    };

    std::filesystem::path m_root;
    std::filesystem::path m_masterDatabase;
    std::filesystem::path m_octoListFile;
    Octo::Proto::Database m_database;
    std::unordered_map<std::string, RegisteredFile> m_files;
    std::optional<ZIPBasedStorage> m_assetbundleZip;
};

#endif


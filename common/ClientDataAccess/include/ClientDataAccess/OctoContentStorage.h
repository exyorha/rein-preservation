#ifndef CLIENT_DATA_ACCESS_OCTO_CONTENT_STORAGE_H
#define CLIENT_DATA_ACCESS_OCTO_CONTENT_STORAGE_H

#include <filesystem>
#include <optional>

#include <Octo/Proto/Database.pb.h>

#include <ClientDataAccess/ZIPBasedStorage.h>

namespace ClientDataAccess {

    class OctoContentStorage {
    public:
        explicit OctoContentStorage(std::filesystem::path &&octoListFile);
        ~OctoContentStorage();

        OctoContentStorage(const OctoContentStorage &other) = delete;
        OctoContentStorage &operator =(const OctoContentStorage &other) = delete;

        inline const Octo::Proto::Database &database() const {
            return m_database;
        }

        [[deprecated("use database().revision()")]] inline int32_t revision() const {
            return m_database.revision();
        }

        std::optional<std::filesystem::path> locateFile(
            const std::string &objectName,
            const std::optional<std::string_view> &md5Sum = {},
            const std::optional<size_t> fileSize = {}) const;

        inline const std::filesystem::path &octoListFile() const {
            return m_octoListFile;
        }

        static std::filesystem::path findNewestOctoList(const std::filesystem::path &gameContentRoot);
        static std::filesystem::path findNewestDatabase(const std::filesystem::path &gameContentRoot);

    private:
        struct RegisteredFile {
            const Octo::Proto::Data *entry;
            bool isResource;
        };

        std::filesystem::path m_octoListFile;
        Octo::Proto::Database m_database;
        std::unordered_map<std::string, RegisteredFile> m_files;
        std::optional<ZIPBasedStorage> m_assetbundleZip;
    };

}

#endif


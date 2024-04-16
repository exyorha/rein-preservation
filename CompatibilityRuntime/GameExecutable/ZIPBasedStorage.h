#ifndef ZIP_BASED_STORAGE_H
#define ZIP_BASED_STORAGE_H

#include <filesystem>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>

struct PhysicalLocationInZIP {
    std::filesystem::path zipFilePath;
    uint64_t fileOffset;
    uint64_t fileLength;
};

class ZIPBasedStorage {
public:
    explicit ZIPBasedStorage(const std::filesystem::path &zipPath);
    ~ZIPBasedStorage();

    ZIPBasedStorage(const ZIPBasedStorage &other) = delete;
    ZIPBasedStorage &operator =(const ZIPBasedStorage &other) = delete;

    std::optional<std::filesystem::path> lookup(const std::string_view &filename) const;

    static std::optional<PhysicalLocationInZIP> unwrapZIPPath(std::filesystem::path &&path);

private:
    struct EndOfCentralDirectory {
        uint32_t signature;
        uint16_t numberOfThisDisk;
        uint16_t numberOfDiskWithTheStartOfTheCentralDirectory;
        uint16_t numberOfCentralDirectoryEntriesOnThisDisk;
        uint16_t totalEntriesInCentralDirectory;
        uint32_t sizeOfCentralDirectory;
        uint32_t offsetOfCentralDirectory;
        uint16_t commentLength;
    } __attribute__((packed));

    struct CentralDirectoryHeader {
        uint32_t signature;
        uint16_t versionMadeBy;
        uint16_t versionNeededToExtract;
        uint16_t generalPurposeBitFlag;
        uint16_t compressionMethod;
        uint16_t lastModifiedTime;
        uint16_t lastModifiedDate;
        uint32_t crc;
        uint32_t compressedSize;
        uint32_t uncompressedSize;
        uint16_t fileNameLength;
        uint16_t extraFieldLength;
        uint16_t fileCommentLength;
        uint16_t diskNumberStart;
        uint16_t internalFileAttributes;
        uint32_t externalFileAttributes;
        uint32_t relativeOffsetOfLocalHeader;
    } __attribute__((packed));


    struct LocalFileHeader {
        uint32_t signature;
        uint16_t versionNeededToExtract;
        uint16_t generalPurposeBitFlag;
        uint16_t compressionMethod;
        uint16_t lastModifiedTime;
        uint16_t lastModifiedDate;
        uint32_t crc;
        uint32_t compressedSize;
        uint32_t uncompressedSize;
        uint16_t fileNameLength;
        uint16_t extraFieldLength;
    } __attribute__((packed));

    static constexpr uint32_t EndOfCentralDirectorySignature = UINT32_C(0x06054b50);
    static constexpr uint32_t LocalFileHeaderSignature = UINT32_C(0x04034b50);
    static constexpr uint32_t CentralDirectoryHeaderSignature = UINT32_C(0x02014b50);

    static constexpr bool LessCompatibleButAvoidReopeningZIPs = true;

    std::filesystem::path m_zipPath;
    uint16_t m_lastDisk;
    std::vector<char> m_centralDirectory;
    std::unordered_map<std::string_view, std::pair<uint16_t, uint32_t>> m_files;
};

#endif


#include <ClientDataAccess/ZIPBasedStorage.h>

#include <cstddef>
#include <ctime>
#include <fstream>
#include <vector>
#include <cstring>
#include <charconv>

namespace ClientDataAccess {

    ZIPBasedStorage::ZIPBasedStorage(const std::filesystem::path &zipPath) : m_zipPath(zipPath) {
        std::ifstream stream;
        stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
        stream.open(zipPath, std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);

        auto fileSize = static_cast<size_t>(stream.tellg());

        if(fileSize < sizeof(EndOfCentralDirectory))
            throw std::runtime_error("the file is too short and wouldn't fit the EndOfCentralDirectory record");

        std::vector<char> commentWithEndOfCentralDirectory(std::min<size_t>(sizeof(EndOfCentralDirectory) + 65535, fileSize));
        stream.seekg(-static_cast<std::ios::off_type>(commentWithEndOfCentralDirectory.size()), std::ios::end);
        stream.read(commentWithEndOfCentralDirectory.data(), commentWithEndOfCentralDirectory.size());

        const char *foundSignature = nullptr;

        for(const char *searchLimit = commentWithEndOfCentralDirectory.data(),
            *searchPtr = searchLimit + commentWithEndOfCentralDirectory.size() - sizeof(EndOfCentralDirectory);
            searchPtr >= searchLimit; searchPtr++) {

            if((searchPtr[0] == static_cast<uint8_t>(EndOfCentralDirectorySignature)) &&
            (searchPtr[1] == static_cast<uint8_t>(EndOfCentralDirectorySignature >> 8)) &&
            (searchPtr[2] == static_cast<uint8_t>(EndOfCentralDirectorySignature >> 16)) &&
            (searchPtr[3] == static_cast<uint8_t>(EndOfCentralDirectorySignature >> 24))) {
                foundSignature = searchPtr;
                break;
            }
        }

        if(!foundSignature) {
            throw std::runtime_error("the 'end of central directory' signature was not found");
        }

        const auto &eocd = *reinterpret_cast<const EndOfCentralDirectory *>(foundSignature);

        if(eocd.commentLength != commentWithEndOfCentralDirectory.data() + commentWithEndOfCentralDirectory.size() - foundSignature - sizeof(EndOfCentralDirectory))
            throw std::runtime_error("the file comment length is inconsistent with the location of the found end of central directory record");

        if(eocd.sizeOfCentralDirectory == UINT32_C(0xFFFFFFFF) || eocd.offsetOfCentralDirectory == UINT32_C(0xFFFFFFFF) || eocd.numberOfThisDisk == 0xFFFF) {
            throw std::runtime_error("the central directory must be addressable without Zip64 extensions");
        }

        if(eocd.numberOfThisDisk != eocd.numberOfDiskWithTheStartOfTheCentralDirectory ||
            eocd.numberOfCentralDirectoryEntriesOnThisDisk != eocd.totalEntriesInCentralDirectory) {
            throw std::runtime_error("the last volume must entirely contain the central directory");
        }

        m_lastDisk = eocd.numberOfThisDisk;

        m_centralDirectory.resize(eocd.sizeOfCentralDirectory);
        stream.seekg(eocd.offsetOfCentralDirectory);
        stream.read(m_centralDirectory.data(), m_centralDirectory.size());

        for(const char *ptr = m_centralDirectory.data(), *limit = ptr + m_centralDirectory.size(); ptr < limit; ) {
            if(limit - ptr < sizeof(CentralDirectoryHeader))
                throw std::runtime_error("truncated header in the central directory");

            const auto &header = *reinterpret_cast<const CentralDirectoryHeader *>(ptr);
            if(header.signature != CentralDirectoryHeaderSignature)
                throw std::runtime_error("bad central directory header signature");

            if(header.compressionMethod != 0 || header.compressedSize != header.uncompressedSize) {
                throw std::runtime_error("the files must not be compressed");
            }

            if(header.uncompressedSize == UINT32_C(0xFFFFFFFF) || header.relativeOffsetOfLocalHeader == UINT32_C(0xFFFFFFFF) ||
                header.diskNumberStart > m_lastDisk)
                throw std::runtime_error("the files must be addressable without Zip64 extensions");

            ptr += sizeof(header);

            auto filename = ptr;

            m_files.emplace(std::string_view(ptr, header.fileNameLength), header);

            ptr += header.fileNameLength + header.extraFieldLength + header.fileCommentLength;
        }
    }

    ZIPBasedStorage::~ZIPBasedStorage() = default;

    std::optional<std::filesystem::path> ZIPBasedStorage::lookup(const std::string_view &filename) const {
        auto it = m_files.find(filename);
        if(it == m_files.end())
            return std::nullopt;

        const auto &header = it->second;

        std::filesystem::path finalPath(m_zipPath);

        std::stringstream suffix;
        if(header.diskNumberStart == m_lastDisk) {
            suffix << ".zip";
        } else {
            suffix << ".z";
            suffix.width(2);
            suffix.fill('0');
            suffix << (header.diskNumberStart + 1);
        }

        uint32_t offset = header.relativeOffsetOfLocalHeader;
        if constexpr (LessCompatibleButAvoidReopeningZIPs) {
            offset += sizeof(LocalFileHeader) + header.fileNameLength + header.extraFieldLength;
        }

        suffix << "@" << offset;

        finalPath.replace_extension(suffix.str());

        return finalPath;
    }

    std::optional<PhysicalLocationInZIP> ZIPBasedStorage::lookupLocation(const std::string_view &filename) const {
        auto it = m_files.find(filename);
        if(it == m_files.end())
            return std::nullopt;

        const auto &header = it->second;

        off_t offset = header.relativeOffsetOfLocalHeader;
        uint32_t length = header.uncompressedSize;

        std::filesystem::path finalPath(m_zipPath);

        std::stringstream suffix;
        if(header.diskNumberStart == m_lastDisk) {
            suffix << ".zip";
        } else {
            suffix << ".z";
            suffix.width(2);
            suffix.fill('0');
            suffix << (header.diskNumberStart + 1);
        }

        finalPath.replace_extension(suffix.str());

        uint16_t dosDate = header.lastModifiedDate;
        uint16_t dosTime = header.lastModifiedTime;

        if constexpr (LessCompatibleButAvoidReopeningZIPs) {
            offset += sizeof(LocalFileHeader) + header.fileNameLength + header.extraFieldLength;
        } else {
            LocalFileHeader fileHeader;
            readLocalHeader(filename, offset, fileHeader, offset);
            length = fileHeader.compressedSize;
            dosDate = fileHeader.lastModifiedDate;
            dosTime = fileHeader.lastModifiedTime;
        }

        struct tm timeparts;
        memset(&timeparts, 0, sizeof(timeparts));

        timeparts.tm_hour = (dosTime >> 11) & 31;
        timeparts.tm_min = (dosTime >> 5) & 63;
        timeparts.tm_sec = (dosTime & 31) << 1;
        timeparts.tm_year = ((dosDate >> 9) & 127) + 80;
        timeparts.tm_mon = ((dosDate >> 5) & 15) - 1;
        timeparts.tm_mday = dosDate & 31;

#ifdef _WIN32
        auto modtime = _mkgmtime(&timeparts);
#else
        auto modtime = timegm(&timeparts);
#endif

        return PhysicalLocationInZIP{
            .zipFilePath = std::move(finalPath),
            .fileOffset = static_cast<uint64_t>(offset),
            .fileLength = length,
            .modtime = modtime
        };
    }

    void ZIPBasedStorage::readLocalHeader(const std::filesystem::path &filename, off_t localHeaderOffset, LocalFileHeader &header,
                                          off_t &dataOffset) {

        std::ifstream stream;
        stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
        stream.open(filename, std::ios::in | std::ios::binary);

        stream.seekg(localHeaderOffset);
        stream.read(reinterpret_cast<char *>(&header), sizeof(header));
        if(header.signature != LocalFileHeaderSignature)
            throw std::runtime_error("bad local file header signature");

        if(header.compressedSize == UINT32_C(0xFFFFFFFF) || header.uncompressedSize == UINT32_C(0xFFFFFFFF)) {
            throw std::runtime_error("files must be addressable without Zip64 extensions");
        }

        if(header.compressionMethod != 0 || header.compressedSize != header.uncompressedSize) {
            throw std::runtime_error("files must not be compressed");
        }

        dataOffset = localHeaderOffset + sizeof(LocalFileHeader) + header.fileNameLength + header.extraFieldLength;
    }

    std::optional<PhysicalLocationInZIP> ZIPBasedStorage::unwrapZIPPath(std::filesystem::path &&path) {
        if(!path.has_extension())
            return std::nullopt;

        auto extensionString = path.extension().string();
        if(!extensionString.starts_with(".z"))
            return std::nullopt;

        auto delim = extensionString.find('@');
        if(delim == std::string::npos)
            return std::nullopt;

        auto offsetBegin = extensionString.data() + delim + 1;
        auto offsetEnd = extensionString.data() + extensionString.size();

        if(offsetBegin == offsetEnd)
            return std::nullopt;

        uint64_t offset;
        auto conversionResult = std::from_chars(offsetBegin, offsetEnd, offset);
        if(conversionResult.ec != std::errc() || conversionResult.ptr != offsetEnd)
            return std::nullopt;

        std::optional<PhysicalLocationInZIP> result;
        auto &location = result.emplace();

        location.zipFilePath = std::move(path);
        extensionString.erase(delim);
        location.zipFilePath.replace_extension(extensionString);

        if constexpr (LessCompatibleButAvoidReopeningZIPs) {
            location.fileOffset = offset;
            location.fileLength = std::string::npos;
        } else {
            LocalFileHeader fileHeader;
            off_t dataOffset;
            readLocalHeader(location.zipFilePath, offset, fileHeader, dataOffset);
            location.fileOffset = dataOffset;
            location.fileLength = fileHeader.compressedSize;
        }

        location.modtime = 0;

        return result;
    }

}

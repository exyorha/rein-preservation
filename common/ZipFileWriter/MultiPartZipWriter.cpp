#include <ZipFileWriter/MultiPartZipWriter.h>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/FileInputOutput.h>

#include <system_error>

#include <zlib.h>

namespace ZipFileWriter {

MultiPartZipWriter::MultiPartZipWriter(const std::filesystem::path &basepath) : m_basepath(basepath), m_centralDirectoryEntries(0) {

}

MultiPartZipWriter::~MultiPartZipWriter() = default;

void MultiPartZipWriter::addFile(const std::string_view &fileName, uint64_t modtimeMicroseconds, const UnityAsset::Stream &data) {
    auto modtimeSeconds = static_cast<time_t>(modtimeMicroseconds / UINT64_C(1000000));
    struct tm modtimeParts;

#ifdef _WIN32
    auto error = gmtime_s(&modtimeParts, &modtimeSeconds);
    if(error != 0) {
        throw std::system_error(error, std::generic_category());
    }
#else
    if(!gmtime_r(&modtimeSeconds, &modtimeParts))
        throw std::system_error(errno, std::generic_category());
#endif

    uint16_t modTimeDOS =
        ((modtimeParts.tm_hour & 31) << 11) |
        ((modtimeParts.tm_min & 63) << 5) |
        ((modtimeParts.tm_sec >> 1) & 31);

    /*
     * struct tm uses year 1900 as the epoch, DOS uses 1980. Assume that
     * we're not going to have pre-1980 timestamps.
     */
    if(modtimeParts.tm_year < 80) {
        modtimeParts.tm_year = 80;
    }

    uint16_t modDateDOS =
        (((modtimeParts.tm_year - 80) & 127) << 9) |
        (((modtimeParts.tm_mon + 1) & 15) << 5) |
        (modtimeParts.tm_mday & 31);

    uint32_t dataCRC = crc32_z(0, data.data(), data.length());

    if(data.length() >= std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("file data length exceeds the maximum allowed for a 32-bit ZIP file");
    }

    if(fileName.size() >= std::numeric_limits<uint16_t>::max()) {
        throw std::runtime_error("the file name is too long");
    }

    UnityAsset::Stream localHeader;
    localHeader << static_cast<uint32_t>(UINT32_C(0x04034b50)); // Local header signature
    localHeader << static_cast<uint16_t>(10); // version needed to extract: 1.0
    localHeader << static_cast<uint16_t>(0x0800); // flags: using UTF-8
    localHeader << static_cast<uint16_t>(0); // compression method: none
    localHeader << modTimeDOS;
    localHeader << modDateDOS;
    localHeader << dataCRC;
    localHeader << static_cast<uint32_t>(data.length()); // compressed size
    localHeader << static_cast<uint32_t>(data.length()); // uncompressed size (same)
    localHeader << static_cast<uint16_t>(fileName.size()); // file name length
    auto extraLengthOffsetInLocalHeader = localHeader.position();
    localHeader << static_cast<uint16_t>(0); // extra field length - will be fixed up if necessary
    localHeader.writeData(reinterpret_cast<const unsigned char *>(fileName.data()), fileName.size());

    /*
     * For spanning purposes; the actual length will always be 1-16 bytes shorter.
     */
    auto sizeOfThisFileData = localHeader.length() + ZipFileAlignment + data.length();

    auto &span = getSpanForDataBytes(sizeOfThisFileData);

    auto startOfLocalHeaderInSpan = static_cast<size_t>(span.tellp());

    auto startOfFileData = startOfLocalHeaderInSpan + localHeader.length();
    auto misalignment = startOfFileData % ZipFileAlignment;
    size_t padding = 0;
    if(misalignment != 0) {
        padding = ZipFileAlignment - misalignment;

        localHeader.setPosition(extraLengthOffsetInLocalHeader);
        localHeader << static_cast<uint16_t>(padding);

        localHeader.setPosition(localHeader.length() + padding);
    }

    span.write(reinterpret_cast<const char *>(localHeader.data()), localHeader.length());
    span.write(reinterpret_cast<const char *>(data.data()), data.length());

    /*
     * Make the central directory entry.
     */
    m_centralDirectory << static_cast<uint32_t>(UINT32_C(0x02014b50)); // Central directory signature
    m_centralDirectory << static_cast<uint16_t>(10); // Made by: DOS, version 1.0
    m_centralDirectory << static_cast<uint16_t>(10); // Version needed: 1.0
    m_centralDirectory << static_cast<uint16_t>(0x0800); // flags: using UTF-8
    m_centralDirectory << static_cast<uint16_t>(0); // compression method: none
    m_centralDirectory << modTimeDOS;
    m_centralDirectory << modDateDOS;
    m_centralDirectory << dataCRC;
    m_centralDirectory << static_cast<uint32_t>(data.length()); // compressed size
    m_centralDirectory << static_cast<uint32_t>(data.length()); // uncompressed size (same)
    m_centralDirectory << static_cast<uint16_t>(fileName.size()); // file name length
    m_centralDirectory << static_cast<uint16_t>(padding); // extra data (alignment padding) length
    m_centralDirectory << static_cast<uint16_t>(0); // comment length
    m_centralDirectory << static_cast<uint16_t>(m_spans.size() - 1); // starting disk (span) number
    m_centralDirectory << static_cast<uint16_t>(0); // internal attributes
    m_centralDirectory << static_cast<uint32_t>(0); // external attributes
    m_centralDirectory << static_cast<uint32_t>(startOfLocalHeaderInSpan); // local header offset
    m_centralDirectory.writeData(reinterpret_cast<const unsigned char *>(fileName.data()), fileName.size());

    /*
     * Also copy the extra data (padding) bytes
     */
    m_centralDirectory.setPosition(m_centralDirectory.length() + padding);

    m_centralDirectoryEntries++;
}

void MultiPartZipWriter::finalize() {
    /*
     * Write the central directory.
     */

    auto &cdSpan = getSpanForDataBytes(m_centralDirectory.length());
    auto cdOffset = static_cast<size_t>(cdSpan.tellp());
    cdSpan.write(reinterpret_cast<const char *>(m_centralDirectory.data()), m_centralDirectory.length());

    if(m_centralDirectory.length() >= std::numeric_limits<uint32_t>::max()) {
        throw std::runtime_error("the central directory is too long");
    }

    auto zip64 = m_centralDirectoryEntries >= std::numeric_limits<uint16_t>::max();

    /*
     * Generate the 'end of central directory' records for all of the spans.
     */
    size_t spanCount = m_spans.size();
    for(size_t spanIndex = 0; spanIndex < spanCount; spanIndex++) {
        auto &span = m_spans[spanIndex];

        UnityAsset::Stream endOfCentralDirectory;

        if(zip64) {
            /*
             * Zip64 end of central directory record
             */
            endOfCentralDirectory << static_cast<uint32_t>(UINT32_C(0x06064b50));
            auto sizeOfZip64EoCOffset = endOfCentralDirectory.length();
            endOfCentralDirectory << static_cast<uint64_t>(0); // will be fixed up

            auto startOfZip64EoCData = endOfCentralDirectory.length();
            endOfCentralDirectory << static_cast<uint16_t>(10); // version made by
            endOfCentralDirectory << static_cast<uint16_t>(10); // version needed to extract
            endOfCentralDirectory << static_cast<uint32_t>(spanIndex); // number of this disk
            endOfCentralDirectory << static_cast<uint32_t>(spanCount - 1); // disk with the central directory

            if(spanIndex == spanCount - 1) {
                endOfCentralDirectory << static_cast<uint64_t>(m_centralDirectoryEntries); // CD entries on this disk
            } else {
                endOfCentralDirectory << static_cast<uint64_t>(0); // CD entries on this disk
            }

            endOfCentralDirectory << static_cast<uint64_t>(m_centralDirectoryEntries); // total CD entries
            endOfCentralDirectory << static_cast<uint64_t>(m_centralDirectory.length()); // CD length
            endOfCentralDirectory << static_cast<uint64_t>(cdOffset); // CD offset on the first CD disk

            auto endOfZip64EoCData = endOfCentralDirectory.length();

            endOfCentralDirectory.setPosition(sizeOfZip64EoCOffset);
            endOfCentralDirectory << static_cast<uint64_t>(endOfZip64EoCData - startOfZip64EoCData);

            endOfCentralDirectory.setPosition(endOfCentralDirectory.length());

            /*
             * End of central directory locator
             */
            endOfCentralDirectory << static_cast<uint32_t>(UINT32_C(0x07064b50)); // signature
            endOfCentralDirectory << static_cast<uint32_t>(spanIndex); // disk with the start of the zip64 EoCD record
            endOfCentralDirectory << static_cast<uint64_t>(span.tellp()); // offset of the start of the zip64 EoCD record on that disk
            endOfCentralDirectory << static_cast<uint32_t>(m_spans.size()); // number of disks
        }

        endOfCentralDirectory << static_cast<uint32_t>(UINT32_C(0x06054b50)); // Central directory signature
        endOfCentralDirectory << static_cast<uint16_t>(spanIndex); // this disk
        endOfCentralDirectory << static_cast<uint16_t>(spanCount - 1); // disk with the central directory

        uint16_t shortEntries = static_cast<uint16_t>(std::min<size_t>(65535, m_centralDirectoryEntries));

        if(spanIndex == spanCount - 1) {
            endOfCentralDirectory << shortEntries; // CD entries on this disk
        } else {
            endOfCentralDirectory << static_cast<uint16_t>(0); // CD entries on this disk
        }

        endOfCentralDirectory << shortEntries; // total CD entries

        endOfCentralDirectory << static_cast<uint32_t>(m_centralDirectory.length()); // CD length

        endOfCentralDirectory << static_cast<uint32_t>(cdOffset); // CD offset on the first CD disk

        endOfCentralDirectory << static_cast<uint16_t>(0); // file comment length (none);

        span.write(reinterpret_cast<const char *>(endOfCentralDirectory.data()), endOfCentralDirectory.length());

        span.close();
    }

    m_spans.clear();
    m_centralDirectory = UnityAsset::Stream();
    m_centralDirectoryEntries = 0;

    /*
     * Rename the last span: .zXX -> .zip
     */

    auto srcName = nameForSpan(spanCount - 1);
    auto dstName(srcName);
    dstName.replace_extension(".zip");

    std::filesystem::rename(srcName, dstName);
}

std::filesystem::path MultiPartZipWriter::nameForSpan(size_t spanIndex) {

    std::stringstream extension;
    extension << ".z";
    extension.width(2);
    extension.fill('0');
    extension << (spanIndex + 1);

    std::filesystem::path spanPath(m_basepath);
    spanPath.replace_extension(extension.str());

    return spanPath;
}

std::ofstream &MultiPartZipWriter::getSpanForDataBytes(size_t bytes) {
    if(bytes > TargetSpanSize) {
        throw std::runtime_error("this chunk of data is larger than the target span size");
    }

    if(m_spans.empty() || static_cast<size_t>(m_spans.back().tellp()) + bytes >TargetSpanSize) {
        auto &newSpan = m_spans.emplace_back();
        newSpan.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);

        if(m_spans.size() >= 100) {
            throw std::runtime_error("too many ZIP spans");
        }

        newSpan.open(nameForSpan(m_spans.size() - 1), std::ios::out | std::ios::trunc | std::ios::binary);

        return newSpan;
    } else {
        return m_spans.back();
    }
}

}

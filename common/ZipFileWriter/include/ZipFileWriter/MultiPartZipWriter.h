#ifndef ZIP_FILE_WRITER_MULTI_PART_ZIP_WRITER_H
#define ZIP_FILE_WRITER_MULTI_PART_ZIP_WRITER_H

#include <filesystem>
#include <fstream>
#include <vector>

#include <UnityAsset/Streams/Stream.h>

namespace ZipFileWriter {

    class MultiPartZipWriter {
    public:
        explicit MultiPartZipWriter(const std::filesystem::path &basepath);
        ~MultiPartZipWriter();

        MultiPartZipWriter(const MultiPartZipWriter &other) = delete;
        MultiPartZipWriter &operator =(const MultiPartZipWriter &other) = delete;

        void addFile(const std::string_view &fileName, uint64_t modtimeMicroseconds, const UnityAsset::Stream &data);

        void finalize();

    private:
        static constexpr unsigned int ZipFileAlignment = 16;
        static constexpr size_t TargetSpanSize = 1900 * 1024 * 1024;

        std::ofstream &getSpanForDataBytes(size_t bytes);

        std::filesystem::path nameForSpan(size_t spanIndex);

        std::filesystem::path m_basepath;
        std::vector<std::ofstream> m_spans;
        UnityAsset::Stream m_centralDirectory;
        size_t m_centralDirectoryEntries;
    };

}

#endif


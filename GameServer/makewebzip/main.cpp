#include <cstdio>

#include <ZipFileWriter/MultiPartZipWriter.h>

#include <UnityAsset/Streams/FileInputOutput.h>

#include <filesystem>
#include <optional>
#include <chrono>

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <OUTPUT ZIP FILE> [INPUT DIRECTORIES...]\n", argv[0]);
        return 1;
    }

    struct Entry {
        std::string nameInZip;
        std::filesystem::path filePath;
        std::filesystem::file_time_type modtime;
    };

    std::vector<Entry> entries;
    std::optional<std::filesystem::file_time_type> mostRecentFile;

    for(int index = 2; index < argc; index++) {
        std::filesystem::path root(argv[index]);

        for(const auto &entry : std::filesystem::recursive_directory_iterator(root)) {
            if(!entry.is_regular_file())
                continue;

            const auto &outEntry = entries.emplace_back(Entry{
                .nameInZip = entry.path().lexically_relative(root).string(),
                .filePath = entry.path(),
                .modtime = entry.last_write_time()
            });

            if(!mostRecentFile.has_value() || *mostRecentFile < entry.last_write_time()) {
                mostRecentFile.emplace(outEntry.modtime);
            }
        }
    }

    std::error_code ec;

    auto zipLastMod = std::filesystem::last_write_time(argv[1], ec);

    if(!ec && (!mostRecentFile.has_value() || *mostRecentFile < zipLastMod)) {
        printf("The zip file %s is up to date.\n", argv[1]);
        return 0;
    }

    printf("Writing the zip file %s\n", argv[1]);

    ZipFileWriter::MultiPartZipWriter writer(argv[1]);
    for(const auto &entry: entries) {
        writer.addFile(
            entry.nameInZip,
            std::chrono::system_clock::to_time_t(std::chrono::clock_cast<std::chrono::system_clock>(entry.modtime))
                * UINT64_C(1000000),
            UnityAsset::Stream(UnityAsset::readFile(entry.filePath)));
    }

    writer.finalize();

    return 0;
}

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <cinttypes>

#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>

#include <Octo/Proto/Database.pb.h>

#include "Octo/Proto/Database.pb.h"
#include "UnityAsset/UnityCompression.h"
#include "conversion_context.h"
#include "bc7e_ispc.h"
#include "multi_part_zip_writer.h"

static int usage(void) {
    fputs(
        "Usage:\n"
        "  process_assets apk2gamedir <SOURCE APK FILE> <TARGET DIRECTORY>\n"
        "  process_assets bundles <SOURCE ASSETBUNDLE DIRECTORY> <TARGET ASSETBUNDLE DIRECTORY>\n"
        "  process_assets packassets <OCTO DATABASE FILE> <CONTENT DIRECTORY> <REPROCESSED ASSET BUNDLE DIRECTORY> <OUTPUT BASE NAME>\n",
        stderr);

    return 1;
}

static UnityAsset::Stream openSourceAssetBundle(
    const std::filesystem::path &contentPath,
    const std::filesystem::path &reprocessedPath,
    const std::string &name) {

    auto reprocessedAsset = reprocessedPath / name;
    if(std::filesystem::exists(reprocessedAsset)) {
        return UnityAsset::Stream(UnityAsset::readFile(reprocessedAsset));
    } else {
        return UnityAsset::Stream(UnityAsset::readFile(contentPath / name));
    }
}

int main(int argc, char **argv) {
    ispc::bc7e_compress_block_init();

    if(argc < 2)
        return usage();

    const char *command = argv[1];

    if(strcmp(command, "apk2gamedir") == 0) {
        if(argc != 4) {
            return usage();
        }

        ConversionContext context(argv[3]);

        context.processAPK(argv[2]);

        context.finalize();
    } else if(strcmp(command, "bundles") == 0) {

        if(argc != 4) {
            return usage();
        }

        auto srcdir = argv[2];
        auto destdir = argv[3];

        for(const auto &entry : std::filesystem::recursive_directory_iterator(srcdir)) {
            if(!entry.is_regular_file())
                continue;

            const auto &path = entry.path();
            if(path.extension() != ".unity3d") {
                continue;
            }

            printf("Processing %s\n", path.c_str());

            UnityAsset::AssetBundleFile file(UnityAsset::Stream(UnityAsset::readFile(path)));

            if(ConversionContext::processAssetBundle(file)) {
                auto out = destdir / path.lexically_relative(srcdir);

                printf("Writing updated %s\n", out.c_str());

                UnityAsset::Stream updated;
                file.serialize(updated);

                std::filesystem::create_directories(out.parent_path());

                UnityAsset::writeFile(out, updated);
            }
        }

    } else if(strcmp(command, "packassets") == 0) {
        if(argc != 6) {
            return usage();
        }

        std::filesystem::path contentPath(argv[3]);
        std::filesystem::path reprocessedPath(argv[4]);

        Octo::Proto::Database octoDB;

        {
            std::ifstream octoDBStream;
            octoDBStream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::eofbit);
            octoDBStream.open(argv[2], std::ios::in | std::ios::binary);
            octoDBStream.exceptions(std::ios::badbit);
            if(!octoDB.ParseFromIstream(&octoDBStream)) {
                fprintf(stderr, "Failed to parse the Octo database\n");
                return 1;
            }
        }

        printf("Loaded the Octo database of revision %d\n", octoDB.revision());

        MultiPartZipWriter writer(argv[5]);

        for(const auto &assetBundle: octoDB.assetbundlelist()) {
            std::string name(assetBundle.name());
            for(char &ch : name) {
                if(ch == ')')
                    ch = '/';
            }

            name.append(".unity3d");

            printf("Asset bundle: %s, modtime: %" PRIu64 ", Unity CRC: %u\n", name.c_str(), assetBundle.generation(), assetBundle.crc());

            auto stream = openSourceAssetBundle(contentPath, reprocessedPath, name);
            UnityAsset::Stream streamToBePacked(stream);
            UnityAsset::AssetBundleFile bundle(std::move(stream));

            if(bundle.assetBundleCRC.value_or(0) != assetBundle.crc()) {
                fprintf(stderr, "Asset bundle CRC mismatch for %s: expected %u, is actually %u\n",
                        name.c_str(), assetBundle.crc(), bundle.assetBundleCRC.value_or(0));
            }

            if(bundle.dataCompression != UnityAsset::UnityCompressionType::LZMA &&
                /*
                 * Audio is mostly incompressible ogg, so don't waste time LZMA'ng it.
                 */
               !name.starts_with("audio/") && !name.starts_with("voice/")) {
                bundle.dataCompression = UnityAsset::UnityCompressionType::LZMA;

                streamToBePacked = UnityAsset::Stream();
                bundle.serialize(streamToBePacked);
            }

            writer.addFile(name, assetBundle.generation(), streamToBePacked);
        }

        writer.finalize();

    } else {
        return usage();
    }


    return 0;
}

#include <cstdio>
#include <filesystem>

#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>

#include "conversion_context.h"
#include "bc7e_ispc.h"

static int usage(void) {
    fputs(
        "Usage:\n"
        "  process_assets apk2gamedir <SOURCE APK FILE> <TARGET DIRECTORY>\n"
        "  process_assets bundles <SOURCE ASSETBUNDLE DIRECTORY> <TARGET ASSETBUNDLE DIRECTORY>\n", stderr);

    return 1;
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
    } else {
        return usage();
    }


    return 0;
}

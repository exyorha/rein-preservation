#include <filesystem>
#include <fstream>
#include <vector>

#include <cstdio>

#include "unzip_handle.h"

#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>
#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/Streams/Stream.h>

class ConversionContext {
public:
    explicit ConversionContext(std::filesystem::path &&targetDirectory);
    ~ConversionContext();

    ConversionContext(const ConversionContext &other) = delete;
    ConversionContext &operator =(const ConversionContext &other) = delete;

    void processAPK(const std::filesystem::path &filename, ZPOS64_T offset = 0, ZPOS64_T length = std::numeric_limits<ZPOS64_T>::max());

    void finalize();

private:
    void unpackCurrentFile(UnzipHandle &apk, const std::filesystem::path &destination);
    void readCurrentFileIntoStream(UnzipHandle &apk, UnityAsset::Stream &stream);

    std::filesystem::path m_targetDirectory;
    std::optional<UnityAsset::AssetBundleFile> m_assetDataFile;
};

ConversionContext::ConversionContext(std::filesystem::path &&targetDirectory) : m_targetDirectory(std::move(targetDirectory)), m_assetDataFile(std::in_place) {
    m_assetDataFile->blockSize = 128 * 1024;
    m_assetDataFile->dataCompression = UnityAsset::UnityCompressionType::LZ4HC;
    m_assetDataFile->directoryCompression = UnityAsset::UnityCompressionType::LZ4HC;
}

ConversionContext::~ConversionContext() = default;

void ConversionContext::finalize() {
    printf("Compressing and writing the asset bundle, this will take some time.\n");

    UnityAsset::Stream serialized;
    m_assetDataFile.value().serialize(serialized);
    m_assetDataFile.reset();

    std::filesystem::create_directories(m_targetDirectory / "NieR_Data");

    UnityAsset::writeFile(m_targetDirectory / "NieR_Data" / "data.unity3d", serialized);
}

void ConversionContext::processAPK(const std::filesystem::path &filename, ZPOS64_T offset, ZPOS64_T length) {
    printf("Processing and unpacking APK file %s\n", filename.filename().generic_string().c_str());

    UnzipHandle apk(filename, offset, length);

    std::string_view assetPrefix = "assets/";
    std::string_view assetDataPrefix = "assets/bin/Data/";
    std::string_view assetManagedPrefix = "assets/bin/Data/Managed/";

    if(apk.goToFirst()) {
        do {
            auto info = apk.getCurrentFileInfo();

            if(info.fileName == "icon.png") {
                unpackCurrentFile(apk, m_targetDirectory / "NieR_Data" / "Resources" / "UnityPlayer.png");

            } else if(info.fileName.ends_with(".apk")) {

                printf("Descending into a nested APK file %s\n", info.fileName.c_str());

                int method;
                apk.openCurrentFile(&method, nullptr, true);

                if(method != 0) {
                    throw std::runtime_error("nested APK files must be uncompressed");
                }

                auto nestedOoffset = apk.getCurrentFileDataOffset();

                processAPK(filename, offset + nestedOoffset, info.compressed_size);

                apk.closeCurrentFile();

            } else if(info.fileName.starts_with(assetManagedPrefix)) {
                auto assetFile = std::string_view(info.fileName).substr(assetManagedPrefix.size());

                unpackCurrentFile(apk, m_targetDirectory / "NieR_Data" / "il2cpp_data" / assetFile);

            } else if(info.fileName.starts_with(assetDataPrefix)) {
                auto assetFile = std::string_view(info.fileName).substr(assetDataPrefix.size());

                if(assetFile.ends_with(".config")) {
                    unpackCurrentFile(apk, m_targetDirectory / "NieR_Data" / assetFile);
                } else {
                    auto extensionPos = assetFile.find_last_of('.');
                    std::string_view extension;
                    if(extensionPos != std::string_view::npos) {
                        extension = assetFile.substr(extensionPos + 1);
                    }

                    if(extension == "split0") {
                        /*
                         * Split data file, first part.
                         */
                        UnityAsset::Stream splitStream;
                        readCurrentFileIntoStream(apk, splitStream);

                        auto directoryPos = apk.getDirectoryPos();

                        /*
                         * Now find and pull the rest.
                         */
                        auto realName = std::string(assetFile.substr(0, extensionPos));

                        unsigned int nextPart = 1;
                        while(true) {
                            std::stringstream nextPartName;
                            nextPartName << assetDataPrefix << realName << ".split" << nextPart;

                            if(!apk.locateFile(nextPartName.str().c_str())) {
                                break;
                            }

                            readCurrentFileIntoStream(apk, splitStream);

                            nextPart++;
                        }

                        apk.seekDirectoryPos(directoryPos);

                        m_assetDataFile.value().entries.emplace_back(std::move(realName), std::move(splitStream), 0);

                    } else if(extension.starts_with("split")) {
                        /*
                         * Split data file, not first part - nothing to do here
                         */
                    } else {
                        UnityAsset::Stream stream;
                        readCurrentFileIntoStream(apk, stream);

                        std::string target;

                        if(assetFile == "unity default resources") {
                            target = "Resources/unity default resources";
                        } else {
                            target = assetFile;
                        }

                        printf("packing up: %s\n", target.c_str());

                        m_assetDataFile.value().entries.emplace_back(std::move(target), std::move(stream), 0);
                    }
                }

            } else if(info.fileName.starts_with(assetPrefix)) {
                auto assetFile = std::string_view(info.fileName).substr(assetPrefix.size());

                if(assetFile.starts_with("Icon-")) {
                    /*
                     * Encryption keys.
                     */

                    unpackCurrentFile(apk, m_targetDirectory / "NieR_Data" / "assets" / assetFile);

                } else if(assetFile.starts_with("Movie/")) {
                    unpackCurrentFile(apk, m_targetDirectory / "NieR_Data" / "StreamingAssets" / assetFile);

                } else {

                    printf("Skipping asset file: '%s'\n", info.fileName.c_str());
                }
            }

        } while(apk.goToNext());
    }
}

void ConversionContext::unpackCurrentFile(UnzipHandle &apk, const std::filesystem::path &destination) {
    std::filesystem::create_directories(destination.parent_path());

    apk.openCurrentFile();

    std::vector<char> chunk(65536);
    {
        std::ofstream stream;
        stream.exceptions(std::ios::failbit | std::ios::badbit | std::ios::badbit);
        stream.open(destination, std::ios::out | std::ios::trunc | std::ios::binary);

        unsigned int chunkLength;
        do {
            chunkLength = apk.readCurrentFile(chunk.data(), chunk.size());

            stream.write(chunk.data(), chunkLength);
        } while(chunkLength != 0);
    }

    apk.closeCurrentFile();
}

void ConversionContext::readCurrentFileIntoStream(UnzipHandle &apk, UnityAsset::Stream &stream) {

    auto info = apk.getCurrentFileInfo();

    auto pos = stream.position();

    stream.setPosition(pos + info.uncompressed_size);
    stream.setPosition(pos);

    apk.openCurrentFile();

    unsigned int chunkLength;
    do {
        chunkLength = std::min<size_t>(std::numeric_limits<int32_t>::max(), stream.length() - stream.position());

        chunkLength = apk.readCurrentFile(const_cast<unsigned char *>(stream.data()) + stream.position(), chunkLength);
        stream.setPosition(stream.position() + chunkLength);
    } while(chunkLength != 0);

    apk.closeCurrentFile();
}

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <SOURCE APK FILE> <TARGET DIRECTORY>\n", argv[0]);
        return 1;
    }

    ConversionContext context(argv[2]);

    context.processAPK(argv[1]);

    context.finalize();


    return 0;
}

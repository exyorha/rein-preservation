#ifndef CONVERSION_CONTEXT_H
#define CONVERSION_CONTEXT_H

#include <unzip.h>

#include <filesystem>
#include <optional>

#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>

class UnzipHandle;

class ConversionContext {
public:
    explicit ConversionContext(std::filesystem::path &&targetDirectory);
    ~ConversionContext();

    ConversionContext(const ConversionContext &other) = delete;
    ConversionContext &operator =(const ConversionContext &other) = delete;

    void processAPK(const std::filesystem::path &filename, ZPOS64_T offset = 0, ZPOS64_T length = std::numeric_limits<ZPOS64_T>::max());

    void finalize();

    static bool processAssetBundle(UnityAsset::AssetBundleFile &bundle);

private:
    void unpackCurrentFile(UnzipHandle &apk, const std::filesystem::path &destination);
    void readCurrentFileIntoStream(UnzipHandle &apk, UnityAsset::Stream &stream);

    std::filesystem::path m_targetDirectory;
    std::optional<UnityAsset::AssetBundleFile> m_assetDataFile;
};

#endif

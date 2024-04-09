#ifndef ASSET_REPROCESSING_H
#define ASSET_REPROCESSING_H

#include <optional>
#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/UnityTextureTypes.h>

namespace UnityAsset::UnityTypes {
    struct SerializedProgram;
}

namespace UnityAsset {
    class SerializedType;
}

class AssetReprocessing {
public:
    static std::optional<UnityAsset::Stream> reprocessAsset(const UnityAsset::SerializedType &type, const UnityAsset::Stream &original);

private:
    using ETCTranscoder = void (*)(size_t width,
                    size_t height,
                    size_t depth,
                    const uint8_t *input,
                    size_t inputRowPitch,
                    size_t inputDepthPitch,
                    uint8_t *output,
                    size_t outputRowPitch,
                    size_t outputDepthPitch);

    static std::optional<UnityAsset::Stream> reprocessShader(const UnityAsset::Stream &original);
    static void addGLCoreSubprograms(UnityAsset::UnityTypes::SerializedProgram &program) ;

    static void checkNoScriptData(const UnityAsset::SerializedType &type);

    static bool rgbaImageHasTransparentPixels(const unsigned char *data, const UnityAsset::TextureSubImage &image);

    struct RebuiltUnityTextureData {
        int32_t newFormat;
        std::vector<unsigned char> newData;
    };

    static std::optional<RebuiltUnityTextureData> reprocessTextureImages(
        const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
        UnityAsset::ColorSpace colorSpace);

    static std::optional<AssetReprocessing::RebuiltUnityTextureData>
        transcodeASTC(const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
                                   UnityAsset::ColorSpace colorSpace);

    static std::optional<AssetReprocessing::RebuiltUnityTextureData>
        transcodeETC(const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
                     UnityAsset::ColorSpace colorSpace, ETCTranscoder transcoder,
                     const UnityAsset::TextureFormatClassification &outputFormat);


};

#endif

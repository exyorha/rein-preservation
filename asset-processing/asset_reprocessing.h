#ifndef ASSET_REPROCESSING_H
#define ASSET_REPROCESSING_H

#include <optional>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/UnityTextureTypes.h>
#include <UnityAsset/StreamedResourceManipulator.h>

namespace UnityAsset::UnityTypes {
    struct SerializedProgram;
}

namespace UnityAsset {
    class SerializedType;
}

struct CollectedApplicationInformation {
    std::optional<std::string> companyName;
    std::optional<std::string> productName;
};

class AssetReprocessing {
public:
    static std::optional<UnityAsset::Stream> reprocessAsset(const UnityAsset::SerializedType &type, const UnityAsset::Stream &original,
                                                            std::optional<UnityAsset::StreamedResourceManipulator> &streamedManipulator,
                                                            bool repackingStreamingData, CollectedApplicationInformation &info);

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
    static void rewriteGLCoreSubprograms(UnityAsset::UnityTypes::SerializedProgram &program) ;

    static void checkNoScriptData(const UnityAsset::SerializedType &type);

    static bool rgbaImageHasTransparentPixels(const unsigned char *data, const UnityAsset::TextureSubImage &image);

    static bool rgbaImageHasTransparentPixels(const float *data, const UnityAsset::TextureSubImage &image);

    struct RebuiltUnityTextureData {
        int32_t newFormat;
        std::vector<unsigned char> newData;
    };

    static std::optional<RebuiltUnityTextureData> reprocessTextureImages(
        const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
        UnityAsset::ColorSpace colorSpace);

    static std::optional<AssetReprocessing::RebuiltUnityTextureData>
        transcodeASTC_LDR(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
                                   UnityAsset::ColorSpace colorSpace);

    static std::optional<AssetReprocessing::RebuiltUnityTextureData>
        transcodeASTC_HDR(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData);

    static std::optional<AssetReprocessing::RebuiltUnityTextureData>
        transcodeETC(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
                     UnityAsset::ColorSpace colorSpace, ETCTranscoder transcoder,
                     const UnityAsset::TextureFormatClassification &outputFormat);


};

#endif

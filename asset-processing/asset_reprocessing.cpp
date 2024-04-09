#include "asset_reprocessing.h"

#include <UnityAsset/UnityTypes.h>
#include <UnityAsset/ShaderBlob.h>
#include <UnityAsset/UnityTextureTypes.h>

#include <UnityAsset/SerializedAsset/SerializedType.h>

#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <regex>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <execution>
#include <ranges>

#include "astc_dec/astc_decomp.h"
#include "rgbcx.h"
#include "etc2_transcoder.h"

void AssetReprocessing::checkNoScriptData(const UnityAsset::SerializedType &type) {

    if(type.m_ScriptID.has_value() || type.m_ScriptTypeIndex >= 0)
        throw std::logic_error("unexpected script data in an asset of type " + std::to_string(type.classID));


}

std::optional<UnityAsset::Stream> AssetReprocessing::reprocessAsset(const UnityAsset::SerializedType &type, const UnityAsset::Stream &original) {
    if(type.classID == UnityAsset::UnityTypes::ShaderClassID) {

        checkNoScriptData(type);

        return reprocessShader(original);

    } else if(type.classID == UnityAsset::UnityTypes::TextureClassID) {

        checkNoScriptData(type);

        throw std::runtime_error("unexpected asset of the base Texture class");

    } else if(type.classID == UnityAsset::UnityTypes::Texture2DClassID) {

        checkNoScriptData(type);

        auto textureAsset = UnityAsset::UnityTypes::deserializeTexture2D(original);

        auto layout = UnityAsset::TextureImageLayout(textureAsset);

        auto result = reprocessTextureImages(layout, textureAsset.image_data, static_cast<UnityAsset::ColorSpace>(textureAsset.m_ColorSpace));
        if(result.has_value()) {
            textureAsset.image_data = std::move(result->newData);
            textureAsset.m_TextureFormat = result->newFormat;
            textureAsset.m_CompleteImageSize = textureAsset.image_data.size();

            return UnityAsset::UnityTypes::serializeTexture2D(textureAsset);
        } else {
            return std::nullopt;
        }

    } else if(type.classID == UnityAsset::UnityTypes::Texture3DClassID) {

        checkNoScriptData(type);

        auto textureAsset = UnityAsset::UnityTypes::deserializeTexture3D(original);

        auto layout = UnityAsset::TextureImageLayout(textureAsset);

        auto result = reprocessTextureImages(layout, textureAsset.image_data, static_cast<UnityAsset::ColorSpace>(textureAsset.m_ColorSpace));
        if(result.has_value()) {
            textureAsset.image_data = std::move(result->newData);
            textureAsset.m_Format = result->newFormat;
            textureAsset.m_DataSize = result->newData.size();

            return UnityAsset::UnityTypes::serializeTexture3D(textureAsset);
        } else {
            return std::nullopt;
        }

    } else if(type.classID == UnityAsset::UnityTypes::Texture2DArrayClassID) {

        checkNoScriptData(type);

        auto textureAsset = UnityAsset::UnityTypes::deserializeTexture2DArray(original);

        auto layout = UnityAsset::TextureImageLayout(textureAsset);

        auto result = reprocessTextureImages(layout, textureAsset.image_data, static_cast<UnityAsset::ColorSpace>(textureAsset.m_ColorSpace));
        if(result.has_value()) {
            textureAsset.image_data = std::move(result->newData);
            textureAsset.m_Format = result->newFormat;
            textureAsset.m_DataSize = result->newData.size();

            return UnityAsset::UnityTypes::serializeTexture2DArray(textureAsset);
        } else {
            return std::nullopt;
        }

    } else if(type.classID == UnityAsset::UnityTypes::CubemapClassID) {

        checkNoScriptData(type);

        auto textureAsset = UnityAsset::UnityTypes::deserializeCubemap(original);

        auto layout = UnityAsset::TextureImageLayout(textureAsset);

        auto result = reprocessTextureImages(layout, textureAsset.image_data, static_cast<UnityAsset::ColorSpace>(textureAsset.m_ColorSpace));
        if(result.has_value()) {
            textureAsset.image_data = std::move(result->newData);
            textureAsset.m_TextureFormat = result->newFormat;
            textureAsset.m_CompleteImageSize = result->newData.size();

            return UnityAsset::UnityTypes::serializeCubemap(textureAsset);
        } else {
            return std::nullopt;
        }

    } else if(type.classID == UnityAsset::UnityTypes::CubemapArrayClassID) {

        checkNoScriptData(type);

        auto textureAsset = UnityAsset::UnityTypes::deserializeCubemapArray(original);

        auto layout = UnityAsset::TextureImageLayout(textureAsset);

        auto result = reprocessTextureImages(layout, textureAsset.image_data, static_cast<UnityAsset::ColorSpace>(textureAsset.m_ColorSpace));
        if(result.has_value()) {
            textureAsset.image_data = std::move(result->newData);
            textureAsset.m_Format = result->newFormat;
            textureAsset.m_DataSize = result->newData.size();

            return UnityAsset::UnityTypes::serializeCubemapArray(textureAsset);
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

void AssetReprocessing::addGLCoreSubprograms(UnityAsset::UnityTypes::SerializedProgram &program) {
    for(size_t index = 0, originalLength = program.m_SubPrograms.size(); index < originalLength; index++) {
        auto &subprogram = program.m_SubPrograms[index];
        if(subprogram.m_GpuProgramType == 4) {

            auto &copy = program.m_SubPrograms.emplace_back(subprogram);
            copy.m_GpuProgramType = 6;
        }
    }
}

std::optional<UnityAsset::Stream> AssetReprocessing::reprocessShader(const UnityAsset::Stream &original) {
    auto shader = UnityAsset::UnityTypes::deserializeShader(original);

    std::optional<uint32_t> glesPlatformIndex;

    if(shader.compressedLengths.size() != shader.decompressedLengths.size() || shader.compressedLengths.size() != shader.offsets.size()) {
        throw std::runtime_error("mismatched number of first-level compressed elements");
    }

    size_t elementCount = shader.compressedLengths.size();

    if(elementCount != shader.platforms.size()) {
        throw std::runtime_error("the number of first-level compressed elements doesn't match the number of platforms");
    }

    for(size_t index = 0, size = shader.platforms.size(); index < size; index++) {
        auto platform = shader.platforms[index];

        if(platform == 15) {
            /*
             * If we already have the GL core platform, then we have nothing to do.
             */
            return std::nullopt;
        }

        if(platform == 9) {
            glesPlatformIndex.emplace(index);
        }
    }

    if(!glesPlatformIndex.has_value()) {
        /*
         * No GLES platform - nothing to convert.
         */

        return std::nullopt;
    }

    printf("AssetReprocessing: adding GL core platform to shader: '%s'\n", shader.m_ParsedForm.m_Name.c_str());

    for(auto &subShader: shader.m_ParsedForm.m_SubShaders) {
        for(auto &pass: subShader.m_Passes) {
            addGLCoreSubprograms(pass.progVertex);
            addGLCoreSubprograms(pass.progFragment);
            addGLCoreSubprograms(pass.progGeometry);
            addGLCoreSubprograms(pass.progHull);
            addGLCoreSubprograms(pass.progDomain);
            addGLCoreSubprograms(pass.progRayTracing);
        }
    }

    UnityAsset::ShaderBlob blob(
        shader.compressedLengths[*glesPlatformIndex],
        shader.decompressedLengths[*glesPlatformIndex],
        shader.offsets[*glesPlatformIndex],
        shader.compressedBlob);

    for(auto &entry: blob.entries) {
        uint32_t version, platform;
        entry >> version >> platform;

        if(version != 201806140)
            throw std::runtime_error("unexpected shader blob version: " + std::to_string(version));

        if(platform == 4) {
            UnityAsset::Stream rebuiltBlob;
            rebuiltBlob << version << static_cast<uint32_t>(6); // GLCore32

            rebuiltBlob.writeData(entry.data() + entry.position(), entry.length() - entry.position());

            /*
             * NOTE: shader blob actually binary, and contains serialized structure
             * interspersed with GLSL data. However, as long as we keep the length
             * intact, we can just modify it in place without deserialization.
             */

            char *sbegin = const_cast<char *>(reinterpret_cast<const char *>(rebuiltBlob.data() + entry.position()));
            char *send = const_cast<char *>(reinterpret_cast<const char *>(rebuiltBlob.data() + rebuiltBlob.length()));

            static std::regex versionRegex("#version ([0-9]+) es\n");
            std::string_view replacementVersion = "#version 150\n";

            for(std::cregex_iterator it(sbegin, send, versionRegex), end; it != end; ++it) {
                const auto &match = *it;

                if(match.length() < replacementVersion.size()) {
                    throw std::runtime_error("not enough space in the shader to fit the replacement length");
                }

                memcpy(sbegin + match.position(), replacementVersion.data(), replacementVersion.size());
                for(size_t indexToFill = match.position() + replacementVersion.size(), limit = match.position() + match.length(); indexToFill < limit;
                    indexToFill++) {

                    sbegin[indexToFill] = ' ';
                }
            }

            entry = rebuiltBlob;
        }
    }

    shader.platforms.emplace_back(15);
    auto &outputCompressedLengths = shader.compressedLengths.emplace_back();
    auto &outputDecompressedLengths = shader.decompressedLengths.emplace_back();
    auto &outputOffsets = shader.offsets.emplace_back();

    blob.serialize(outputCompressedLengths, outputDecompressedLengths, outputOffsets, shader.compressedBlob);

    return UnityAsset::UnityTypes::serializeShader(shader);
}

bool AssetReprocessing::rgbaImageHasTransparentPixels(const unsigned char *data, const UnityAsset::TextureSubImage &image) {

    const unsigned char *rowStart = data + image.offset();

    for(unsigned int row = 0; row < image.storageInfo().activeHeight(); row++) {
        for(unsigned int pixel = 0; pixel < image.storageInfo().activeWidth(); pixel++) {
            if(rowStart[pixel * 4 + 3] != 0xFF) {
                return true;
            }
        }

        rowStart += image.storageInfo().storedWidth() * 4;
    }

    return false;
}



std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::transcodeETC(const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
            UnityAsset::ColorSpace colorSpace, ETCTranscoder transcoder, const UnityAsset::TextureFormatClassification &outputFormat) {

    auto recompressed = layout.reformat(outputFormat);

    std::vector<unsigned char> storage(recompressed.totalDataSize());


    for(size_t imageIndex = 0, imageCount = layout.images().size(); imageIndex < imageCount; imageIndex++) {
        const auto &sourceImage = layout.images()[imageIndex];
        const auto &destinationImage = recompressed.images()[imageIndex];

        transcoder(sourceImage.storageInfo().activeWidth(), sourceImage.storageInfo().activeHeight(), 1,
                   textureImageData.data() + sourceImage.offset(),
                   sourceImage.storageInfo().storedWidth() / layout.format().blockWidth() * layout.format().blockSizeBytes(),
                   sourceImage.storageInfo().dataLength(),
                   storage.data() + destinationImage.offset(),
                   destinationImage.storageInfo().storedWidth() / recompressed.format().blockWidth() * recompressed.format().blockSizeBytes(),
                   destinationImage.storageInfo().dataLength());
    }

    return AssetReprocessing::RebuiltUnityTextureData{
        .newFormat = static_cast<int32_t>(outputFormat.unityFormat()),
        .newData = std::move(storage)
    };

}
std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::transcodeASTC(const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
                                   UnityAsset::ColorSpace colorSpace) {


    auto inputBlockWidth = layout.format().blockWidth();
    auto inputBlockHeight = layout.format().blockHeight();
    auto inputBlockSizeBytes = layout.format().blockSizeBytes();

    auto uncompressed = layout.reformat(UnityAsset::TextureFormatClassification::RGBA8, true);

    printf("uncompressed image layout, total %zu bytes:\n", uncompressed.totalDataSize());

    for(const auto &image: uncompressed.images()) {
        printf("  - active size: %ux%u, storage size: %ux%u, data length: %u, offset: %u\n",
               image.storageInfo().activeWidth(), image.storageInfo().activeHeight(),
               image.storageInfo().storedWidth(), image.storageInfo().storedHeight(),
               image.storageInfo().dataLength(), image.offset());
    }

    std::vector<unsigned char> storage(uncompressed.totalDataSize());


    std::vector<unsigned char> singleblock(4 * inputBlockWidth * inputBlockHeight);

    for(size_t imageIndex = 0, imageCount = layout.images().size(); imageIndex < imageCount; imageIndex++) {
        const auto &sourceImage = layout.images()[imageIndex];
        const auto &destinationImage = uncompressed.images()[imageIndex];

        unsigned int blocksX = sourceImage.storageInfo().storedWidth() / inputBlockWidth;
        unsigned int blocksY = sourceImage.storageInfo().storedHeight() / inputBlockHeight;

        const unsigned char *sourceData = textureImageData.data() + sourceImage.offset();
        unsigned char *destinationData = storage.data() + destinationImage.offset();

        for(unsigned int blockY = 0; blockY < blocksY; blockY++) {
            for(unsigned int blockX = 0; blockX < blocksX; blockX++) {
                auto blockSourceData = sourceData + blockY * blocksX * inputBlockSizeBytes + blockX * inputBlockSizeBytes;

                auto blockDestinationData = destinationData + sizeof(uint32_t) * (
                    blockY * inputBlockHeight * blocksX * inputBlockWidth +
                    blockX * inputBlockWidth
                );

                auto singleBlock = singleblock.data();

                basisu::astc::decompress(singleBlock, blockSourceData,
                                         colorSpace == UnityAsset::ColorSpace::Gamma,
                                         inputBlockWidth,
                                         inputBlockHeight);

                for(unsigned int blockLine = 0; blockLine < inputBlockHeight; blockLine++) {
                    memcpy(blockDestinationData, singleBlock, 4 * inputBlockWidth);
                    singleBlock += 4 * inputBlockWidth;
                    blockDestinationData += 4 * blocksX * inputBlockWidth;
                }
            }
        }
    }

    bool hasTransparentPixels = false;
    for(const auto &image: uncompressed.images()) {
        if(rgbaImageHasTransparentPixels(storage.data(), image)) {
            hasTransparentPixels = true;
            break;
        }
    }

    auto outputFormat = &UnityAsset::TextureFormatClassification::DXT1;

    if(hasTransparentPixels) {
        printf("At least one image has transparent pixels, will encode into DXT5\n");

        outputFormat = &UnityAsset::TextureFormatClassification::DXT5;
    } else {
        printf("No images have transparent pixels, will encode into DXT1\n");
    }

    auto compressedLayout = layout.reformat(*outputFormat);

    auto outputBlockWidth = compressedLayout.format().blockWidth();
    auto outputBlockHeight = compressedLayout.format().blockHeight();
    auto outputBlockSize = compressedLayout.format().blockSizeBytes();

    printf("compressed image layout, %ux%u blocks of %u bytes, total %zu bytes:\n",
           outputBlockWidth,
           outputBlockHeight,
           outputBlockSize,
           compressedLayout.totalDataSize());

    for(const auto &image: compressedLayout.images()) {
        printf("  - active size: %ux%u, storage size: %ux%u, data length: %u, offset: %u\n",
               image.storageInfo().activeWidth(), image.storageInfo().activeHeight(),
               image.storageInfo().storedWidth(), image.storageInfo().storedHeight(),
               image.storageInfo().dataLength(), image.offset());
    }

    std::vector<unsigned char> compressedStorage(compressedLayout.totalDataSize());

    for(size_t imageIndex = 0, imageCount = compressedLayout.images().size(); imageIndex < imageCount; imageIndex++) {
        const auto &inputImage = uncompressed.images()[imageIndex];
        const auto &outputImage = compressedLayout.images()[imageIndex];

        auto inputData = storage.data() + inputImage.offset();
        auto outputData = compressedStorage.data() + outputImage.offset();

        auto outputBlocksX = outputImage.storageInfo().storedWidth() / outputBlockWidth;
        auto outputBlocksY = outputImage.storageInfo().storedHeight() / outputBlockHeight;
        auto totalOutputBlocks = outputBlocksX *outputBlocksY;

        printf("  %u X blocks, %u Y blocks, %u blocks total\n", outputBlocksX, outputBlocksY, totalOutputBlocks);

        std::vector<unsigned int> scanRange;
        scanRange.reserve(totalOutputBlocks);
        for(unsigned int blockIndex = 0; blockIndex <totalOutputBlocks; blockIndex++) {
            scanRange.push_back(blockIndex);
        }

        std::for_each(std::execution::par_unseq, scanRange.begin(), scanRange.end(), [outputBlocksX, inputData, &inputImage, &storage, &outputImage,
            outputFormat, outputData](unsigned int blockNumber) {
            auto blockX = blockNumber % outputBlocksX;
            auto blockY = blockNumber / outputBlocksX;

            std::array<uint8_t, 4 * 4 * 4> rgba;

            unsigned int pixelX = blockX * 4;
            unsigned int pixelY = blockY * 4;

            for(unsigned int bpY = 0; bpY < 4; bpY++) {
                for(unsigned int bpX = 0; bpX < 4; bpX++) {
                    auto fullX = pixelX + bpX;
                    auto fullY = pixelY + bpY;

                    auto pixel = inputData + 4 * (inputImage.storageInfo().storedWidth() * fullY + fullX);

                    rgba[bpY * 16 + bpX * 4 + 0] = pixel[0];
                    rgba[bpY * 16 + bpX * 4 + 1] = pixel[1];
                    rgba[bpY * 16 + bpX * 4 + 2] = pixel[2];
                    rgba[bpY * 16 + bpX * 4 + 3] = pixel[3];
                }
            }
            if(outputFormat == &UnityAsset::TextureFormatClassification::DXT1) {
                rgbcx::encode_bc1(18, outputData + 8 * blockNumber, rgba.data(), true, false);
            } else {
                rgbcx::encode_bc3(18, outputData + 16 * blockNumber, rgba.data());
            }
        });
    }

    return RebuiltUnityTextureData{
        .newFormat = static_cast<int32_t>(outputFormat->unityFormat()),
        .newData = std::move(compressedStorage)
    };
}

std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::reprocessTextureImages(const UnityAsset::TextureImageLayout &layout, const std::vector<unsigned char> &textureImageData,
                                   UnityAsset::ColorSpace colorSpace) {

    auto inputBlockWidth = layout.format().blockWidth();
    auto inputBlockHeight = layout.format().blockHeight();
    auto inputBlockSizeBytes = layout.format().blockSizeBytes();

    printf("input image set: encoding class %d, %ux%u pixel blocks of %u bytes, total %zu bytes:\n",
           static_cast<int32_t>(layout.format().encodingClass()),
           inputBlockWidth, inputBlockHeight,
           layout.format().blockSizeBytes(),
           layout.totalDataSize());

    for(const auto &image: layout.images()) {
        printf("  - active size: %ux%u, storage size: %ux%u, data length: %u, offset: %u\n",
               image.storageInfo().activeWidth(), image.storageInfo().activeHeight(),
               image.storageInfo().storedWidth(), image.storageInfo().storedHeight(),
               image.storageInfo().dataLength(), image.offset());
    }

    auto encoding = layout.format().encodingClass();

    if(encoding == UnityAsset::TextureEncodingClass::A8 ||
        encoding == UnityAsset::TextureEncodingClass::RGBA8 ||
        encoding == UnityAsset::TextureEncodingClass::ARGB8 ||
        encoding == UnityAsset::TextureEncodingClass::RGB8 ||
        encoding == UnityAsset::TextureEncodingClass::RGBA4444) {
        /*
         * Good for desktop.
         */
        return std::nullopt;
    } else if(encoding == UnityAsset::TextureEncodingClass::ASTC_LDR) {
        return transcodeASTC(layout, textureImageData, colorSpace);

    } else if(encoding == UnityAsset::TextureEncodingClass::ETC1) {
        if(colorSpace == UnityAsset::ColorSpace::Gamma) {
            return transcodeETC(layout, textureImageData, colorSpace, &etc2transcoder::LoadETC2SRGB8ToBC1,
                                       UnityAsset::TextureFormatClassification::DXT1);
        } else {
            return transcodeETC(layout, textureImageData, colorSpace, &etc2transcoder::LoadETC2RGB8ToBC1,
                                       UnityAsset::TextureFormatClassification::DXT1);
        }

    } else if(encoding == UnityAsset::TextureEncodingClass::ETC2_RGBA) {
        if(colorSpace == UnityAsset::ColorSpace::Gamma) {
            return transcodeETC(layout, textureImageData, colorSpace, &etc2transcoder::LoadETC2SRGBA8ToBC3, UnityAsset::TextureFormatClassification::DXT5);
        } else {
            return transcodeETC(layout, textureImageData, colorSpace, &etc2transcoder::LoadETC2RGBA8ToBC3, UnityAsset::TextureFormatClassification::DXT5);
        }

    } else {
        throw std::runtime_error("transcoding of this encoding class is not supported");
    }
}


#include "asset_reprocessing.h"

#include <UnityAsset/UnityTypes.h>
#include <UnityAsset/ShaderBlob.h>
#include <UnityAsset/UnityTextureTypes.h>

#include <UnityAsset/SerializedAsset/SerializedType.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h>

#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <regex>
#include <cstring>
#include <algorithm>
#include <execution>
#include <unordered_set>

#include "UnityAsset/StreamedResourceManipulator.h"
#include "astc_dec/astc_decomp.h"
#include "crunch/crn_defs.h"
#include "etc2_transcoder.h"
#include "HalfFloat.h"
#include "crunch/crn_decomp.h"

#ifdef USE_BC7E
#include "bc7e_ispc.h"
#else
#include "bc7enc_rdo/bc7enc.h"
#endif

void AssetReprocessing::checkNoScriptData(const UnityAsset::SerializedType &type) {

    if(type.m_ScriptID.has_value() || type.m_ScriptTypeIndex >= 0)
        throw std::logic_error("unexpected script data in an asset of type " + std::to_string(type.classID));


}

static const unsigned char *getImageData(size_t expectedImageSize, const std::vector<unsigned char> &embeddedData,
                                         std::optional<UnityAsset::StreamedResourceManipulator> &streamedManipulator,
                                         const UnityAsset::UnityTypes::StreamingInfo &streaming,
                                         bool &streamed) {
    if(embeddedData.size() == expectedImageSize) {
        streamed = false;
        return embeddedData.data();
    } else if(embeddedData.empty() && streaming.size == expectedImageSize && streamedManipulator.has_value()) {

        printf("Streaming access: offset %u, length %u\n", streaming.offset, streaming.size);

        streamed = true;

        return streamedManipulator->getViewOfOriginalData(streaming.offset, streaming.size).data();
    } else {
        std::stringstream stream;
        stream << "Unsupported texture streaming configuration. Expected image size: " <<expectedImageSize << ", size of inline data: " << embeddedData.size()
            << ", streamed resource file: ";

        if(streamedManipulator.has_value()) {
            stream << " present";
        } else {
            stream << " not present";
        }

        stream << ", file name in the streaming info: " << streaming.path << ", offset: " << streaming.offset << ", length: " << streaming.size;

        throw std::runtime_error(stream.str());
    }
}

static void storeStreamedImageData(UnityAsset::UnityTypes::StreamingInfo &streaming,
                                   UnityAsset::StreamedResourceManipulator &streamedManipulator,
                                   std::vector<unsigned char> &&data) {

    streamedManipulator.consumeRangeOfOriginalData(streaming.offset, streaming.size);

    streaming.size = data.size();
    streaming.offset = streamedManipulator.addNewData(std::move(data));
}


static void storeStreamedImageData(UnityAsset::UnityTypes::StreamingInfo &streaming,
                                   UnityAsset::StreamedResourceManipulator &streamedManipulator,
                                   const unsigned char *data, size_t dataSize) {

    return storeStreamedImageData(streaming, streamedManipulator, std::vector<unsigned char>(data, data + dataSize));
}

static void storeStreamedImageData(UnityAsset::UnityTypes::StreamingInfo &streaming,
                                   UnityAsset::StreamedResourceManipulator &streamedManipulator,
                                   const UnityAsset::Stream &data) {

    return storeStreamedImageData(streaming, streamedManipulator, data.data(), data.length());
}

static void pruneMethodList(std::vector<int32_t> &indices, const std::unordered_set<int32_t> &methodsToBeDisabled) {
    auto end = std::remove_if(indices.begin(), indices.end(), [&methodsToBeDisabled](int32_t index) {
        return methodsToBeDisabled.contains(index);
    });

    indices.erase(end, indices.end());
}

template<typename T>
static inline std::unique_ptr<T> deserializeClass(const UnityAsset::Stream &stream) {
    std::unique_ptr<T> result = std::make_unique<T>();
    result->deserialize(stream);
    return result;
}

template<typename T>
static inline UnityAsset::Stream serializeClass(T &object) {
    UnityAsset::Stream stream;
    object.serialize(stream);
    return stream;
}

template<typename T>
static inline UnityAsset::Stream serializeClass(std::unique_ptr<T> &object) {
    return serializeClass(*object);
}

std::optional<UnityAsset::Stream> AssetReprocessing::reprocessAsset(const UnityAsset::SerializedType &type, const UnityAsset::Stream &original,
                                                                    std::optional<UnityAsset::StreamedResourceManipulator> &streamedManipulator,
                                                                    bool repackingStreamingData, CollectedApplicationInformation &info) {
    if(type.classID == UnityAsset::UnityClasses::Shader::ClassID) {

        if(repackingStreamingData)
            return std::nullopt;

        checkNoScriptData(type);

        return reprocessShader(original);

    } else if(type.classID == UnityAsset::UnityClasses::Texture::ClassID) {

        checkNoScriptData(type);

        throw std::runtime_error("unexpected asset of the base Texture class");

    } else if(type.classID == UnityAsset::UnityClasses::Texture2D::ClassID) {

        checkNoScriptData(type);

        auto textureAsset = deserializeClass<UnityAsset::UnityClasses::Texture2D>(original);

        bool streamed;
        std::optional<std::vector<unsigned char>> uncrunchedData;

        if(textureAsset->m_TextureFormat == UnityAsset::TextureFormat::ETC2_RGBA8Crunched) {
            if(repackingStreamingData)
                throw std::logic_error("should never get the second chance call for the crunched data");

            struct CrunchUnpackContentDeleter {
                inline void operator()(crnd::crnd_unpack_context context) {
                    crnd::crnd_free(context);
                }
            };

            auto crunchedData = getImageData(textureAsset->m_CompleteImageSize, textureAsset->image_data, streamedManipulator, textureAsset->m_StreamData,
                                             streamed);

            crnd::crn_file_info fileInfo;
            memset(&fileInfo, 0, sizeof(fileInfo));
            fileInfo.m_struct_size = sizeof(fileInfo);

            if(!crnd::crnd_validate_file(crunchedData, textureAsset->m_CompleteImageSize, &fileInfo))
                throw std::runtime_error("the crunched data has failed to validate");

            crnd::crn_texture_info textureInfo;
            memset(&textureInfo, 0, sizeof(textureInfo));
            textureInfo.m_struct_size = sizeof(textureInfo);

            if(!crnd::crnd_get_texture_info(crunchedData, textureAsset->m_CompleteImageSize, &textureInfo)) {
                throw std::runtime_error("failed to get the texture info from the crunched data");
            }

            if(textureInfo.m_width != textureAsset->m_Width || textureInfo.m_height != textureAsset->m_Height ||
                textureInfo.m_levels != textureAsset->m_MipCount || textureInfo.m_faces != 1) {

                throw std::runtime_error("the data in the crunch header is inconsistent with the data in the asset");
            }

            std::unique_ptr<std::remove_pointer<crnd::crnd_unpack_context>::type, CrunchUnpackContentDeleter> context(
                crnd::crnd_unpack_begin(crunchedData, textureAsset->m_CompleteImageSize)
            );

            if(!context) {
                throw std::runtime_error("failed to initialize the Crunch unpacker");
            }

            crnd::crn_level_info levelInfo;
            memset(&levelInfo, 0, sizeof(levelInfo));
            levelInfo.m_struct_size = sizeof(levelInfo);

            auto &output = uncrunchedData.emplace();
            for(uint32_t level = 0; level < textureInfo.m_levels; level++) {
                if(!crnd::crnd_get_level_info(crunchedData, textureAsset->m_CompleteImageSize, level, &levelInfo))
                    throw std::runtime_error("failed to get the crunched mipmap level info");

                auto levelPitch = levelInfo.m_blocks_x * levelInfo.m_bytes_per_block;
                auto levelSize = levelInfo.m_blocks_y * levelPitch;

                auto offset = output.size();
                output.resize(offset + levelSize);

                void *dest = output.data() + offset;

                if(!crnd::crnd_unpack_level(context.get(), &dest, levelSize, levelPitch, level))
                    throw std::runtime_error("failed to unpack a crunched mip level");
            }

            textureAsset->m_CompleteImageSize = output.size();
            textureAsset->m_TextureFormat = UnityAsset::TextureFormat::ETC2_RGBA8;
        }

        UnityAsset::TextureImageLayout layout(*textureAsset);

        const unsigned char *sourceData;

        if(uncrunchedData.has_value()) {
            sourceData = uncrunchedData->data();
        } else {
            sourceData = getImageData(layout.totalDataSize(), textureAsset->image_data, streamedManipulator, textureAsset->m_StreamData, streamed);
        }

        if(repackingStreamingData) {
            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, sourceData, layout.totalDataSize());
            } else {
                return std::nullopt;
            }
        } else {
            auto result = reprocessTextureImages(layout, sourceData, static_cast<UnityAsset::ColorSpace>(textureAsset->m_ColorSpace));

            if(!result.has_value())
                return std::nullopt;

            textureAsset->m_TextureFormat = result->newFormat;
            textureAsset->m_CompleteImageSize = result->newData.size();

            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, std::move(result->newData));

            } else {

                textureAsset->image_data = std::move(result->newData);
            }
        }

        return serializeClass(textureAsset);

    } else if(type.classID == UnityAsset::UnityClasses::Texture3D::ClassID) {

        checkNoScriptData(type);

        auto textureAsset = deserializeClass<UnityAsset::UnityClasses::Texture3D>(original);

        UnityAsset::TextureImageLayout layout(*textureAsset);

        bool streamed;
        auto sourceData = getImageData(layout.totalDataSize(), textureAsset->image_data, streamedManipulator, textureAsset->m_StreamData, streamed);

        if(repackingStreamingData) {
            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, sourceData, layout.totalDataSize());
            } else {
                return std::nullopt;
            }
        } else {

            auto result = reprocessTextureImages(layout, sourceData, static_cast<UnityAsset::ColorSpace>(textureAsset->m_ColorSpace));
            if(!result.has_value())
                return std::nullopt;

            textureAsset->m_Format = result->newFormat;
            textureAsset->m_DataSize = result->newData.size();

            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, std::move(result->newData));
            } else {

                textureAsset->image_data = std::move(result->newData);
            }
        }

        return serializeClass(textureAsset);

    } else if(type.classID == UnityAsset::UnityClasses::Texture2DArray::ClassID) {

        checkNoScriptData(type);

        auto textureAsset = deserializeClass<UnityAsset::UnityClasses::Texture2DArray>(original);

        UnityAsset::TextureImageLayout layout(*textureAsset);

        bool streamed;
        auto sourceData = getImageData(layout.totalDataSize(), textureAsset->image_data, streamedManipulator, textureAsset->m_StreamData, streamed);

        if(repackingStreamingData) {
            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, sourceData, layout.totalDataSize());
            } else {
                return std::nullopt;
            }
        } else {

            auto result = reprocessTextureImages(layout, sourceData, static_cast<UnityAsset::ColorSpace>(textureAsset->m_ColorSpace));
            if(!result.has_value())
                return std::nullopt;


            textureAsset->m_Format = result->newFormat;
            textureAsset->m_DataSize = result->newData.size();

            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, std::move(result->newData));
            } else {

                textureAsset->image_data = std::move(result->newData);
            }

        }

        return serializeClass(textureAsset);

    } else if(type.classID == UnityAsset::UnityClasses::Cubemap::ClassID) {

        checkNoScriptData(type);

        auto textureAsset = deserializeClass<UnityAsset::UnityClasses::Cubemap>(original);

        auto &cubemapData = static_cast<UnityAsset::UnityTypes::Cubemap &>(*textureAsset);

        UnityAsset::TextureImageLayout layout(cubemapData);

        bool streamed;
        auto sourceData = getImageData(layout.totalDataSize(), cubemapData.image_data, streamedManipulator, cubemapData.m_StreamData, streamed);

        if(repackingStreamingData) {
            if(streamed) {
                storeStreamedImageData(cubemapData.m_StreamData, *streamedManipulator, sourceData, layout.totalDataSize());
            } else {
                return std::nullopt;
            }
        } else {

            auto result = reprocessTextureImages(layout, sourceData, static_cast<UnityAsset::ColorSpace>(cubemapData.m_ColorSpace));
            if(!result.has_value())
                return std::nullopt;

            cubemapData.m_TextureFormat = result->newFormat;
            cubemapData.m_CompleteImageSize = result->newData.size() / cubemapData.m_ImageCount;

            if(streamed) {
                storeStreamedImageData(cubemapData.m_StreamData, *streamedManipulator, std::move(result->newData));
            } else {

                cubemapData.image_data = std::move(result->newData);
            }
        }

        return serializeClass(textureAsset);

    } else if(type.classID == UnityAsset::UnityClasses::CubemapArray::ClassID) {

        checkNoScriptData(type);

        auto textureAsset = deserializeClass<UnityAsset::UnityClasses::CubemapArray>(original);

        UnityAsset::TextureImageLayout layout(*textureAsset);

        bool streamed;
        auto sourceData = getImageData(layout.totalDataSize(), textureAsset->image_data, streamedManipulator, textureAsset->m_StreamData, streamed);

        if(repackingStreamingData) {
            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, sourceData, layout.totalDataSize());
            } else {
                return std::nullopt;
            }
        } else {


            auto result = reprocessTextureImages(layout, sourceData, static_cast<UnityAsset::ColorSpace>(textureAsset->m_ColorSpace));
            if(!result.has_value())
                return std::nullopt;

            textureAsset->m_Format = result->newFormat;
            textureAsset->m_DataSize = result->newData.size();

            if(streamed) {
                storeStreamedImageData(textureAsset->m_StreamData, *streamedManipulator, std::move(result->newData));
            } else {

                textureAsset->image_data = std::move(result->newData);
            }
        }

        return serializeClass(textureAsset);

    } else if(type.classID == UnityAsset::UnityClasses::Mesh::ClassID && repackingStreamingData) {
        checkNoScriptData(type);

        auto meshAsset = deserializeClass<UnityAsset::UnityClasses::Mesh>(original);

        if(meshAsset->m_StreamData.size == 0 || !streamedManipulator.has_value()) {
            return std::nullopt;
        }

        printf("Rewriting the streamed data of the mesh asset '%s' \n", meshAsset->m_Name.c_str());

        auto originalData = streamedManipulator->getViewOfOriginalData(meshAsset->m_StreamData.offset, meshAsset->m_StreamData.size);

        storeStreamedImageData(meshAsset->m_StreamData, *streamedManipulator, originalData);

        return serializeClass(meshAsset);

    } else if(type.classID == UnityAsset::UnityClasses::UnityConnectSettings::ClassID) {
        checkNoScriptData(type);

        auto settings = deserializeClass<UnityAsset::UnityClasses::UnityConnectSettings>(original);
        settings->m_Enabled = false;
        settings->m_EventOldUrl.clear();
        settings->m_EventUrl.clear();
        settings->m_ConfigUrl.clear();
        settings->vCrashReportingSettings.m_Enabled = false;
        settings->vCrashReportingSettings.m_EventUrl.clear();
        settings->vUnityPurchasingSettings.m_Enabled = false;
        settings->vUnityAnalyticsSettings.m_Enabled = false;
        settings->vUnityAdsSettings.m_Enabled = false;
        settings->vUnityAdsSettings.m_GameId.clear();
        settings->vPerformanceReportingSettings.m_Enabled = false;

        return serializeClass(settings);

    } else if(type.classID == UnityAsset::UnityClasses::BuildSettings::ClassID) {
        checkNoScriptData(type);

        auto settings = deserializeClass<UnityAsset::UnityClasses::BuildSettings>(original);

        /*
         * Add the desktop OpenGL as supported, and set it as preferred by
         * listing it first. Keep the original GLES API too, because we
         * are perfectly able to run on GLES too, as long as it's a desktop
         * or Tegra.
         */
        // GraphicsDeviceType::OpenGLCore
        settings->m_GraphicsAPIs.emplace(settings->m_GraphicsAPIs.begin(), 17);

        return serializeClass(settings);

    } else if(type.classID == UnityAsset::UnityClasses::RuntimeInitializeOnLoadManager::ClassID) {
        checkNoScriptData(type);

        auto settings = deserializeClass<UnityAsset::UnityClasses::RuntimeInitializeOnLoadManager>(original);

        std::unordered_set<int32_t> methodsToBeDisabled;

        /*
         * If desired, the debugger could be disabled with:
         * Assembly-CSharp namespace SRDebugger class AutoInitialize method OnLoad
         */

        for(size_t index = 0, count = settings->m_ClassMethodInfos.size(); index < count; index++) {
            const auto &method = settings->m_ClassMethodInfos[index];

            const auto &classDef = settings->m_ClassInfos.at(method.m_ClassIndex);
            const auto &assemblyName = settings->m_AssemblyNames.at(classDef.m_AssemblyNameIndex);
            const auto &namespaceName = settings->m_NamespaceNames.at(classDef.m_NamespaceIndex);

            if(classDef.m_ClassName == "IronSourceInitilizer" /* sic */ ||
                assemblyName.starts_with("UnityEngine.Purchasing") ||
                assemblyName.starts_with("Unity.Services")
            ) {
                printf("disabling the initialization call to assembly %s namespace %s class %s method %s\n",
                    assemblyName.c_str(),
                    namespaceName.c_str(),
                    classDef.m_ClassName.c_str(),
                    method.m_MethodName.c_str());


                methodsToBeDisabled.emplace(index);
            }
        }

        pruneMethodList(settings->m_BeforeUnityMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_AfterUnityMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_BeforeMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_AfterMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_AfterAssembliesLoadedUnityMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_AfterAssembliesLoadedMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_BeforeSplashScreenUnityMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_BeforeSplashScreenMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_SubsystemRegistrationUnityMethodExecutionOrders, methodsToBeDisabled);
        pruneMethodList(settings->m_SubsystemRegistrationMethodExecutionOrders, methodsToBeDisabled);

        return serializeClass(settings);

    } else if(type.classID == UnityAsset::UnityClasses::PlayerSettings::ClassID) {

        checkNoScriptData(type);

        auto settings = deserializeClass<UnityAsset::UnityClasses::PlayerSettings>(original);

        settings->submitAnalytics = false;

        settings->resolutionScalingMode = 0;

        info.companyName.emplace(settings->companyName);
        info.productName.emplace(settings->productName);

        return serializeClass(settings);

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
    auto shader = deserializeClass<UnityAsset::UnityClasses::Shader>(original);

    std::optional<uint32_t> glesPlatformIndex;

    if(shader->compressedLengths.size() != shader->decompressedLengths.size() || shader->compressedLengths.size() != shader->offsets.size()) {
        throw std::runtime_error("mismatched number of first-level compressed elements");
    }

    size_t elementCount = shader->compressedLengths.size();

    if(elementCount != shader->platforms.size()) {
        throw std::runtime_error("the number of first-level compressed elements doesn't match the number of platforms");
    }

    for(size_t index = 0, size = shader->platforms.size(); index < size; index++) {
        auto platform = shader->platforms[index];

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

    printf("AssetReprocessing: adding GL core platform to shader: '%s'\n", shader->m_ParsedForm.m_Name.c_str());

    for(auto &subShader: shader->m_ParsedForm.m_SubShaders) {
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
        shader->compressedLengths[*glesPlatformIndex],
        shader->decompressedLengths[*glesPlatformIndex],
        shader->offsets[*glesPlatformIndex],
        shader->compressedBlob);

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

    shader->platforms.emplace_back(15);
    auto &outputCompressedLengths = shader->compressedLengths.emplace_back();
    auto &outputDecompressedLengths = shader->decompressedLengths.emplace_back();
    auto &outputOffsets = shader->offsets.emplace_back();

    blob.serialize(outputCompressedLengths, outputDecompressedLengths, outputOffsets, shader->compressedBlob);

    return serializeClass(shader);
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

bool AssetReprocessing::rgbaImageHasTransparentPixels(const float *data, const UnityAsset::TextureSubImage &image) {

    auto rowStart = reinterpret_cast<const float *>(reinterpret_cast<const unsigned char *>(data) + image.offset());

    for(unsigned int row = 0; row < image.storageInfo().activeHeight(); row++) {
        for(unsigned int pixel = 0; pixel < image.storageInfo().activeWidth(); pixel++) {
            if(rowStart[pixel * 4 + 3] < 1.0f) {
                return true;
            }
        }

        rowStart += image.storageInfo().storedWidth() * 4;
    }

    return false;
}

std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::transcodeETC(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
            UnityAsset::ColorSpace colorSpace, ETCTranscoder transcoder, const UnityAsset::TextureFormatClassification &outputFormat) {

    auto recompressed = layout.reformat(outputFormat);

    std::vector<unsigned char> storage(recompressed.totalDataSize());


    for(size_t imageIndex = 0, imageCount = layout.images().size(); imageIndex < imageCount; imageIndex++) {
        const auto &sourceImage = layout.images()[imageIndex];
        const auto &destinationImage = recompressed.images()[imageIndex];

        transcoder(sourceImage.storageInfo().activeWidth(), sourceImage.storageInfo().activeHeight(), 1,
                   textureImageData + sourceImage.offset(),
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
    AssetReprocessing::transcodeASTC_LDR(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
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

        const unsigned char *sourceData = textureImageData + sourceImage.offset();
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

    auto compressedLayout = layout.reformat(UnityAsset::TextureFormatClassification::BC7);

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
        auto totalOutputBlocks = outputBlocksX * outputBlocksY;

        printf("  %u X blocks, %u Y blocks, %u blocks total\n", outputBlocksX, outputBlocksY, totalOutputBlocks);

        unsigned int sourcePitchBytes = sizeof(uint32_t) * inputImage.storageInfo().storedWidth();

#ifdef USE_BC7E
        ispc::bc7e_compress_block_params params;
        if(layout.format().blockWidth() <= 6 || layout.format().blockHeight() <= 6) {
            ispc::bc7e_compress_block_params_init_veryslow(&params, colorSpace == UnityAsset::ColorSpace::Gamma);
        } else {
            ispc::bc7e_compress_block_params_init_basic(&params, colorSpace == UnityAsset::ColorSpace::Gamma);
        }

        static const unsigned int BC7Batch = 64;

        std::vector<unsigned int> scanRange;
        for(unsigned int blockIndex = 0; blockIndex < totalOutputBlocks; blockIndex += BC7Batch) {
            scanRange.push_back(blockIndex);
        }

        std::for_each(std::execution::par_unseq, scanRange.begin(), scanRange.end(), [totalOutputBlocks, outputData,
                        &params, outputBlocksX, inputData, sourcePitchBytes](unsigned int firstBlockNumber) {

            unsigned int blocksInBatch = std::min<unsigned int>(totalOutputBlocks - firstBlockNumber, BC7Batch);

            std::vector<uint32_t> batchPixels(blocksInBatch * 4 * 4);

            for(unsigned int blockIndex = 0; blockIndex < blocksInBatch; blockIndex++) {
                auto blockNumber = firstBlockNumber + blockIndex;

                uint32_t *blockPixels = batchPixels.data() + 16 * blockIndex;

                auto blockX = blockNumber % outputBlocksX;
                auto blockY = blockNumber / outputBlocksX;

                const uint8_t *pixel = inputData + sourcePitchBytes * (blockY * 4) + sizeof(uint32_t) * blockX * 4;

                for(unsigned int scanline = 0; scanline < 4; scanline++) {
                    memcpy(blockPixels, pixel, 4 * sizeof(uint32_t));
                    blockPixels += 4;
                    pixel += sourcePitchBytes;
                }
            }

            ispc::bc7e_compress_blocks(
                blocksInBatch,
                reinterpret_cast<uint64_t *>(outputData + 16 * firstBlockNumber),
                batchPixels.data(),
                &params);
        });
#else
        bc7enc_compress_block_params params;
        bc7enc_compress_block_params_init(&params);
        bc7enc_compress_block_params_init_linear_weights(&params);
        params.m_uber_level = BC7ENC_MAX_UBER_LEVEL;

        std::vector<unsigned int> scanRange(totalOutputBlocks);
        for(size_t index = 0; index < scanRange.size(); index++) {
            scanRange[index] = index;
        }

        std::for_each(std::execution::par_unseq, scanRange.begin(), scanRange.end(), [totalOutputBlocks, outputData,
                        &params, outputBlocksX, inputData, sourcePitchBytes](unsigned int blockIndex) {

            auto blockX = blockIndex % outputBlocksX;
            auto blockY = blockIndex / outputBlocksX;

            std::array<uint32_t, 4*4> blockPixels;

            const uint8_t *pixel = inputData + sourcePitchBytes * (blockY * 4) + sizeof(uint32_t) * blockX * 4;

            for(unsigned int scanline = 0; scanline < 4; scanline++) {
                memcpy(&blockPixels[scanline * 4], pixel, 4 * sizeof(uint32_t));
                pixel += sourcePitchBytes;
            }


            bc7enc_compress_block(
                reinterpret_cast<uint64_t *>(outputData + 16 * blockIndex),
                blockPixels.data(),
                &params);
        });
#endif
    }

    return RebuiltUnityTextureData{
        .newFormat = static_cast<int32_t>(compressedLayout.format().unityFormat()),
        .newData = std::move(compressedStorage)
    };
}

std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::transcodeASTC_HDR(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData) {


    auto inputBlockWidth = layout.format().blockWidth();
    auto inputBlockHeight = layout.format().blockHeight();
    auto inputBlockSizeBytes = layout.format().blockSizeBytes();

    auto uncompressed = layout.reformat(UnityAsset::TextureFormatClassification::RGBA32F, true);

    printf("uncompressed image layout, total %zu bytes:\n", uncompressed.totalDataSize());

    for(const auto &image: uncompressed.images()) {
        printf("  - active size: %ux%u, storage size: %ux%u, data length: %u, offset: %u\n",
               image.storageInfo().activeWidth(), image.storageInfo().activeHeight(),
               image.storageInfo().storedWidth(), image.storageInfo().storedHeight(),
               image.storageInfo().dataLength(), image.offset());
    }

    std::vector<unsigned char> storage(uncompressed.totalDataSize());

    std::vector<float> singleblock(4 * inputBlockWidth * inputBlockHeight);

    for(size_t imageIndex = 0, imageCount = layout.images().size(); imageIndex < imageCount; imageIndex++) {
        const auto &sourceImage = layout.images()[imageIndex];
        const auto &destinationImage = uncompressed.images()[imageIndex];

        unsigned int blocksX = sourceImage.storageInfo().storedWidth() / inputBlockWidth;
        unsigned int blocksY = sourceImage.storageInfo().storedHeight() / inputBlockHeight;

        const unsigned char *sourceData = textureImageData + sourceImage.offset();
        unsigned char *destinationData = storage.data() + destinationImage.offset();

        for(unsigned int blockY = 0; blockY < blocksY; blockY++) {
            for(unsigned int blockX = 0; blockX < blocksX; blockX++) {
                auto blockSourceData = sourceData + blockY * blocksX * inputBlockSizeBytes + blockX * inputBlockSizeBytes;

                auto blockDestinationData = destinationData + sizeof(uint32_t) * (
                    blockY * inputBlockHeight * blocksX * inputBlockWidth +
                    blockX * inputBlockWidth
                );

                auto singleBlock = singleblock.data();

                basisu::astc::decompressFloat(singleBlock, blockSourceData,
                                         inputBlockWidth,
                                         inputBlockHeight, false);

                for(unsigned int blockLine = 0; blockLine < inputBlockHeight; blockLine++) {
                    memcpy(blockDestinationData, singleBlock, 16 * inputBlockWidth);
                    singleBlock += 16 * inputBlockWidth;
                    blockDestinationData += 16 * blocksX * inputBlockWidth;
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

    if(hasTransparentPixels) {
        /*
         * We can't encode an ASTC HDR image that uses alpha into anything but
         * R16G16B16A16_SFLOAT, there is just no format for it on the desktop.
         */
        printf("HDR ASTC image using the alpha channel, encoding into R16G16B16A16_SFLOAT\n");

        const auto &outputFormat = UnityAsset::TextureFormatClassification::RGBA16F;
        auto encodedLayout = layout.reformat(outputFormat);

        std::vector<unsigned char> encodedStorage(encodedLayout.totalDataSize());

        for(size_t imageIndex = 0, imageCount = encodedLayout.images().size(); imageIndex < imageCount; imageIndex++) {
            const auto &inputImage = uncompressed.images()[imageIndex];
            const auto &outputImage = encodedLayout.images()[imageIndex];

            auto inputData = reinterpret_cast<const float *>(storage.data() + inputImage.offset());
            auto outputData = reinterpret_cast<uint16_t *>(encodedStorage.data() + outputImage.offset());

            unsigned int rows = inputImage.storageInfo().activeHeight();
            unsigned int columns = inputImage.storageInfo().activeWidth();

            for(unsigned int y = 0; y < rows; y++) {
                for(unsigned int x = 0; x < columns; x++) {
                    auto floatData = inputData + 4 * (y * inputImage.storageInfo().storedWidth() + x);
                    auto halfData  = outputData + 4 * (y * outputImage.storageInfo().storedWidth() + x);

                    halfData[0] = floatToHalfFloat(floatData[0]);
                    halfData[1] = floatToHalfFloat(floatData[1]);
                    halfData[2] = floatToHalfFloat(floatData[2]);
                    halfData[3] = floatToHalfFloat(floatData[3]);
                }
            }
        }


        return AssetReprocessing::RebuiltUnityTextureData{
            .newFormat = outputFormat.unityFormat(),
            .newData = std::move(encodedStorage)
        };

    } else {
        /*
         * This can be encoded into BC6H.
         */
        throw std::runtime_error("fully opaque ASTC HDR image\n");
    }
}


std::optional<AssetReprocessing::RebuiltUnityTextureData>
    AssetReprocessing::reprocessTextureImages(const UnityAsset::TextureImageLayout &layout, const unsigned char *textureImageData,
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
        encoding == UnityAsset::TextureEncodingClass::R8 ||
        encoding == UnityAsset::TextureEncodingClass::RGBA8 ||
        encoding == UnityAsset::TextureEncodingClass::ARGB8 ||
        encoding == UnityAsset::TextureEncodingClass::RGB8 ||
        encoding == UnityAsset::TextureEncodingClass::RGBA4444 ||
        layout.images().empty()) {
        /*
         * Good for desktop.
         */
        return std::nullopt;
    } else if(encoding == UnityAsset::TextureEncodingClass::ASTC_LDR) {
        return transcodeASTC_LDR(layout, textureImageData, colorSpace);

    } else if(encoding == UnityAsset::TextureEncodingClass::ASTC_HDR) {
        return transcodeASTC_HDR(layout, textureImageData);


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


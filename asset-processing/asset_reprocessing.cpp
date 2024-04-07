#include "asset_reprocessing.h"

#include <UnityAsset/UnityTypes.h>
#include <UnityAsset/ShaderBlob.h>

#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <regex>
#include <cstring>

static void addGLCoreSubprograms(UnityAsset::UnityTypes::SerializedProgram &program) {
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

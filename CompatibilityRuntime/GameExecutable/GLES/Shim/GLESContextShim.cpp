#include <GLES/Shim/GLESContextShim.h>

#include <GLES/TextureEmulation/EmulatedTextureFormat.h>
#include <GLES2/gl2ext.h>

#include <array>
#include <cstring>
#include <charconv>

#include <GLES/TextureEmulation/stb_dxt.h>

bool GLESContextShim::AlwaysEmulateASTC = false;
bool GLESContextShim::NeverRecompressASTC = false;

GLESContextShim::GLESContextShim(std::unique_ptr<BaseGLESContext> &&nextContext) : m_nextContext(std::move(nextContext)),
    m_emulatedASTC(false), m_recompressASTC(false), m_warnedAboutEmulatingTextures(false) {

}

GLESContextShim::~GLESContextShim() = default;

void *GLESContextShim::getProcAddress(const char *name) noexcept {
    try {

        lateInitialize();

        if(strcmp(name, "glGetString") == 0) {
            return reinterpret_cast<void *>(shim_glGetString);

        } else if(strcmp(name, "glGetStringi") == 0) {
            return reinterpret_cast<void *>(shim_glGetStringi);

        } else if(strcmp(name, "glGetIntegerv") == 0) {
            return reinterpret_cast<void *>(shim_glGetIntegerv);

        } else if(strcmp(name, "glCompressedTexImage2D") == 0) {
            return reinterpret_cast<void *>(shim_glCompressedTexImage2D);

        } else if(strcmp(name, "glCompressedTexSubImage2D") == 0) {
            return reinterpret_cast<void *>(shim_glCompressedTexSubImage2D);

        } else if(strcmp(name, "glCompressedTexSubImage3D") == 0) {
            return reinterpret_cast<void *>(shim_glCompressedTexSubImage3D);


        } else if(strcmp(name, "glTexStorage2D") == 0) {
            return reinterpret_cast<void *>(shim_glTexStorage2D);

        } else if(strcmp(name, "glTexStorage3D") == 0) {
            return reinterpret_cast<void *>(shim_glTexStorage3D);
        } else {

            return m_nextContext->getProcAddress(name);
        }
    } catch(const std::exception &e) {
        fprintf(stderr, "GLESContextShim::getProcAddress(%s): exception: %s\n", name, e.what());
        return nullptr;
    }
}

void GLESContextShim::lateInitialize() {

    if(!m_nextSymbols.has_value())
        m_nextSymbols.emplace(*m_nextContext);

    if(!m_extensionString.has_value()) {
        m_extensionString.emplace(*m_nextSymbols);

        if(!m_extensionString->hasExtension("GL_KHR_texture_compression_astc_ldr")) {

            m_extensionString->addExtension("GL_KHR_texture_compression_astc_ldr");

            m_emulatedASTC = true;
        }

        if(AlwaysEmulateASTC)
            m_emulatedASTC = true;

        if(m_emulatedASTC) {
            m_recompressASTC = !NeverRecompressASTC &&
                m_extensionString->hasExtension("GL_EXT_texture_compression_s3tc") && m_extensionString->hasExtension("GL_EXT_texture_sRGB");
        }
    }
}

GLESContextShim *GLESContextShim::getAndInitializeShim() {
    auto shim = getCurrentShim();
    if(shim) {
        shim->lateInitialize();
    }

    return shim;
}

const GLubyte *GL_APIENTRY GLESContextShim::shim_glGetString(GLenum name) {
    return getAndInitializeShim()->m_nextSymbols->glGetString(name);
}

const GLubyte *GL_APIENTRY GLESContextShim::shim_glGetStringi(GLenum name, GLuint index) {
    auto shim = getAndInitializeShim();
    if(name == GL_EXTENSIONS) {
        return shim->m_extensionString->extension(index);
    } else {
        return shim->m_nextSymbols->glGetStringi(name, index);
    }
}

void GL_APIENTRY GLESContextShim::shim_glGetIntegerv(GLenum pname, GLint *data) {
    auto shim = getAndInitializeShim();

    if(pname == GL_NUM_EXTENSIONS) {
        *data = shim->m_extensionString->numberOfExtensions();
    } else {
        shim->m_nextSymbols->glGetIntegerv(pname, data);
    }
}

/*
 * We are going to optimistically assume that the pixel unpack buffers are not used.
 */

std::vector<unsigned char> GLESContextShim::decompressTexture(const EmulatedTextureFormat *emulationPtr,
                                                              GLsizei width, GLsizei height, const void *data) {

    if(!m_warnedAboutEmulatingTextures) {
        fprintf(stderr,
                "\n"
                "PERFORMANCE WARNING:\n"
                "GLESContextShim: an ASTC-compressed texture (GL internal format 0x%04X) is being decompressed on the CPU, either because of the lack of the GPU support for ASTC, or because enforced emulation was explicitly requested. Be advised that this is a slow codepath, and the loading times will be significantly increased.\n"
        "\n"
        "If possible, the ASTC-containing texture assets should be recompressed ahead of time.\n"
        "\n"
        "This warning will only be shown once per session.\n"
        "\n", emulationPtr->internalFormat);

        if(m_recompressASTC) {
            fprintf(stderr,
                    "\n"
                    "VISUAL QUALITY WARNING:\n"
                    "GLESContextShim: the emulated ASTC textures are being recompressed into BC texture encodng. This can be disabled with a launch option, if the VRAM budget allows. Uncompressed emulated ASTC textures should be expected to require 9x the VRAM of the original, while BC-recompressed should only require 2.25x.\n"
                    "\n");
        }

        m_warnedAboutEmulatingTextures = true;
    }

    /*
     * This structure is tiny, and copying it to the stack allows the compiler
     * to avoid the unnecessary reloads from it, caused by the weak aliasing rules.
     */
    auto emulation = *emulationPtr;

    auto widthInBlocks = (width + emulation.blockSizeX - 1) / emulation.blockSizeX;
    auto heightInBlocks = (height + emulation.blockSizeY - 1) / emulation.blockSizeY;

    auto blockAlignedWidth = widthInBlocks * emulation.blockSizeX;
    auto blockAlignedHeight = heightInBlocks * emulation.blockSizeY;

    std::vector<unsigned char> outputRaster(width * height * 4);

    const unsigned char *sourceData = static_cast<const unsigned char *>(data);
    unsigned char *outputRow = outputRaster.data();

    for(unsigned int blockRow = 0; blockRow < heightInBlocks; blockRow++) {
        unsigned char *outputColumn = outputRow;

        for(unsigned int blockColumn = 0; blockColumn < widthInBlocks; blockColumn++) {
            std::array<unsigned char, 12*12*4> block;

            emulation.decode(&emulation, sourceData, block.data());
            sourceData += emulation.blockSizeBytes;

            auto pixelsToCopyX = std::min<unsigned int>(emulation.blockSizeX, width - blockColumn * emulation.blockSizeX);
            auto pixelsToCopyY = std::min<unsigned int>(emulation.blockSizeY, height - blockRow * emulation.blockSizeY);

            unsigned char *output = outputColumn;
            for(unsigned int pixelRow = 0; pixelRow < pixelsToCopyY; pixelRow++) {
                memcpy(output, block.data() + pixelRow * emulation.blockSizeX * 4, pixelsToCopyX * 4);

                output += width * 4;
            }

            outputColumn += emulation.blockSizeX * 4;
        }

        outputRow += width * emulation.blockSizeY * 4;
    }

    return outputRaster;
}

void GL_APIENTRY GLESContextShim::shim_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                                              GLsizei height, GLint border, GLsizei imageSize, const void *data) {

    auto shim = getAndInitializeShim();

    const EmulatedTextureFormat *emulation = nullptr;

    if(shim->m_emulatedASTC) {
        emulation = getEmulatedTextureFormat(internalformat);
    }

    if(!emulation) {
        return shim->m_nextSymbols->glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    }

    auto outputRaster = shim->decompressTexture(emulation, width, height, data);

    if(shim->m_recompressASTC && acceptableImageDimensionsForCompression(width, height)) {
        auto compressed = compressToDXT5(width, height, outputRaster.data());

        GLenum format;
        if(emulation->isSRGB) {
            format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        } else {
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }

        shim->m_nextSymbols->glCompressedTexImage2D(target, level, format, width, height, 0,
                                                       compressed.size(), compressed.data());

    } else {
        GLenum internalFormat;
        if(emulation->isSRGB) {
            internalFormat = GL_SRGB8_ALPHA8;
        } else {
            internalFormat = GL_RGBA8;
        }
        shim->m_nextSymbols->glTexImage2D(target, level, internalFormat, width, height, 0,
                                                    GL_RGBA, GL_UNSIGNED_BYTE, outputRaster.data());
    }
}


void GL_APIENTRY GLESContextShim::shim_glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                                                 GLsizei height, GLenum format, GLsizei imageSize, const void *data) {

    auto shim = getAndInitializeShim();

    const EmulatedTextureFormat *emulation = nullptr;

    if(shim->m_emulatedASTC) {
        emulation = getEmulatedTextureFormat(format);
    }

    if(!emulation) {
        return shim->m_nextSymbols->glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    }

    auto outputRaster = shim->decompressTexture(emulation, width, height, data);

    if(shim->m_recompressASTC && acceptableImageDimensionsForCompression(width, height)) {
        auto compressed = compressToDXT5(width, height, outputRaster.data());

        GLenum format;
        if(emulation->isSRGB) {
            format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
        } else {
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }

        shim->m_nextSymbols->glCompressedTexSubImage2D(target, level, xoffset, yoffset,
                                                       width,
                                                       height, format,
                                                       compressed.size(), compressed.data());
    } else {

        shim->m_nextSymbols->glTexSubImage2D(target, level, xoffset, yoffset, width, height,
                                                        GL_RGBA, GL_UNSIGNED_BYTE, outputRaster.data());
    }
}

void GL_APIENTRY GLESContextShim::shim_glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
                                                                 GLsizei width, GLsizei height, GLsizei depth, GLenum format,
                                                                 GLsizei imageSize, const void *data) {

    auto emulation = getEmulatedTextureFormat(format);
    if(!emulation) {
        return getAndInitializeShim()->m_nextSymbols->glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    }

    /*
     * ASTC has extra special support for volume textures.
     */

    fprintf(stderr, "shim_glCompressedTexSubImage3D: 3D texture upload (of format 0x%04X) in the need of emulation, but 3D emulation is not implemented\n",
            format);

    getAndInitializeShim()->m_nextSymbols->glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

GLenum GLESContextShim::subsituteStorageInternalFormat(GLenum internalformat, bool canCompress) const {

    GLenum lowerInternalFormat = internalformat;

    if(m_emulatedASTC) {
        auto emulation = getEmulatedTextureFormat(internalformat);
        if(emulation) {
            if(m_recompressASTC && canCompress) {
                if(emulation->isSRGB) {
                    lowerInternalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
                } else {
                    lowerInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                }
            } else {
                if(emulation->isSRGB) {
                    lowerInternalFormat = GL_SRGB8_ALPHA8;
                } else {
                    lowerInternalFormat = GL_RGBA8;
                }
            }
        }
    }

    return lowerInternalFormat;

}

void GL_APIENTRY GLESContextShim::shim_glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {

    auto shim = getAndInitializeShim();
    shim->m_nextSymbols->glTexStorage2D(target, levels,
                                        shim->subsituteStorageInternalFormat(internalformat,
                                                                             acceptableImageDimensionsForCompression(width, height)), width, height);

}

void GL_APIENTRY GLESContextShim::shim_glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) {

    auto shim = getAndInitializeShim();
    shim->m_nextSymbols->glTexStorage3D(target, levels,
                                        shim->subsituteStorageInternalFormat(internalformat,
                                        acceptableImageDimensionsForCompression(width, height)), width, height, depth);
}

std::vector<unsigned char> GLESContextShim::compressToDXT5(GLsizei width, GLsizei height, const unsigned char *rgbaData) {
    unsigned int outputWidthInBlocks = (width + 3) / 4;
    unsigned int outputHeightInBlocks = (height + 3) / 4;

    std::vector<unsigned char> output(outputWidthInBlocks * outputHeightInBlocks * 16);

    auto outputPtr = output.data();

    auto inputRow = rgbaData;

    for(unsigned int blockY = 0; blockY < outputHeightInBlocks; blockY++) {
        auto input = inputRow;

        for(unsigned int blockX = 0; blockX < outputWidthInBlocks; blockX++) {
            std::array<unsigned char, 4*4*4> blockPixels;

            unsigned int rowsInThisBlock = std::min<unsigned int>(4, height - blockY * 4);
            unsigned int columnBytesInThisBlock = 4 * std::min<unsigned int>(4, width - blockX * 4);

            for(unsigned int blockPixelY = 0; blockPixelY < rowsInThisBlock; blockPixelY++) {
                memcpy(blockPixels.data() + 16 * blockPixelY, input + blockPixelY * width * 4, columnBytesInThisBlock);
            }

            stb_compress_dxt_block(outputPtr, blockPixels.data(), 1, STB_DXT_HIGHQUAL);

            input += 4 * 4;
            outputPtr += 16;
        }

        inputRow += 4 * 4 * width;
    }

    return output;
}

bool GLESContextShim::acceptableImageDimensionsForCompression(unsigned int width, unsigned int height) {
    return
        (width == 1 || width == 2 || (width & 3) == 0) &&
        (height == 1 || height == 2 || (height & 3) == 0);
}

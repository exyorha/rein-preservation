#include <GLES3/gl32.h>

#include <GLES/TextureEmulation/EmulatedTextureFormat.h>
#include <GLES/TextureEmulation/astc_dec/astc_decomp.h>

#include <cstdio>

static void decompressASTC(const EmulatedTextureFormat *format, const unsigned char *compressedData, unsigned char *uncompressedRasterData);

static const EmulatedTextureFormat EmulatedTextureFormats[]{
    { GL_COMPRESSED_RGBA_ASTC_4x4, 4, 4, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_5x4, 5, 4, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_5x5, 5, 5, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_6x5, 6, 5, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_6x6, 6, 6, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_8x5, 8, 5, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_8x6, 8, 6, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_8x8, 8, 8, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_10x5, 10, 5, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_10x6, 10, 6, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_10x8, 10, 8, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_10x10, 10, 10, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_12x10, 12, 10, 16, false, decompressASTC },
    { GL_COMPRESSED_RGBA_ASTC_12x12, 12, 12, 16, false, decompressASTC },

    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4, 4, 4, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4, 5, 4, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5, 5, 5, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5, 6, 5, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6, 6, 6, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5, 8, 5, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6, 8, 6, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8, 8, 8, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5, 10, 5, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6, 10, 6, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8, 10, 8, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10, 10, 10, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10, 12, 10, 16, true, decompressASTC },
    { GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12, 12, 12, 16, true, decompressASTC },
};

const EmulatedTextureFormat *getEmulatedTextureFormat(uint16_t format) {
    for(const auto &desc: EmulatedTextureFormats) {
        if(desc.internalFormat == format)
            return &desc;
    }

    return nullptr;
}

static void decompressASTC(const EmulatedTextureFormat *format, const unsigned char *compressedData, unsigned char *uncompressedRasterData) {
    basisu::astc::decompress(
        uncompressedRasterData,
        compressedData,
        format->isSRGB,
        format->blockSizeX,
        format->blockSizeY);
}

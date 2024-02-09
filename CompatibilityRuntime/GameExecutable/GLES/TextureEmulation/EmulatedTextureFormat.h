#ifndef GLES_TEXTURE_EMULATION_EMULATED_TEXTURE_FORMAT_H
#define GLES_TEXTURE_EMULATION_EMULATED_TEXTURE_FORMAT_H

#include <cstdint>

struct EmulatedTextureFormat;

using DecodeSingleTextureBlock = void (*)(const EmulatedTextureFormat *format, const unsigned char *compressedData, unsigned char *uncompressedRasterData);

struct EmulatedTextureFormat {
    uint16_t internalFormat; // format as requested by the game engine
    uint16_t substitutedInternalFormat; // format that should be passed to the host OpenGL
    uint8_t blockSizeX;
    uint8_t blockSizeY;
    uint8_t blockSizeBytes;
    bool isSRGB;
    DecodeSingleTextureBlock decode;
};

const EmulatedTextureFormat *getEmulatedTextureFormat(uint16_t format);

#endif


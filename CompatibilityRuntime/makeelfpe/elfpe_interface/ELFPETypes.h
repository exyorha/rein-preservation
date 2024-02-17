#ifndef ELFPETYPES_H
#define ELFPETYPES_H

#include <cstdint>

struct ELFPEHeader {
    uint32_t signature;
    uint32_t elfOffset;
    uint32_t phdrRVA;
    uint32_t phdrSize;

    static constexpr uint32_t ELFPESignature = 0x50464c45;
};

#endif

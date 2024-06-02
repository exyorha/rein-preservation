#include <ELF/Image.h>
#include <ELF/ElfModule.h>

#include <algorithm>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <cstring>

#include <system_error>

#include "support.h"
#include <Translator/thunking.h>
#include "SystemAPIThunking.h"
#include "GlobalContext.h"

Image::Image(const std::filesystem::path &path) :
    m_mapper(path),
    m_initArray(nullptr), m_initArraySize(0),
    m_finiArray(nullptr), m_finiArraySize(0),
    m_hash(nullptr),
    m_stringTable(nullptr), m_stringTableSize(0),
    m_symbolTable(nullptr),
    m_got(nullptr),
    m_relocations(nullptr), m_relocationsSize(0),
    m_pltRelocations(nullptr), m_pltRelocationsSize(0),
    m_path(path) {


    parseDynamic();

    if(m_relocations && m_relocationsSize > 0)
        processRelocations(m_relocations, m_relocationsSize);

    if(m_pltRelocations && m_pltRelocationsSize > 0)
        processRelocations(m_pltRelocations, m_pltRelocationsSize);
}

void Image::runConstructors() {
    if(m_initArray && m_initArraySize > 0) {
        auto numberOfFunctions = m_initArraySize / sizeof(InitFiniFunc);
        for(auto func = m_initArray, limit = func + numberOfFunctions; func < limit; ++func) {
            armcall(*func);
        }
    }
}

void Image::parseDynamic() {
    if(!m_mapper.dynamic())
        throw std::runtime_error("the image has no dynamic table");

    for(auto entry = m_mapper.dynamic(); entry->d_tag != DT_NULL; ++entry) {
        switch(entry->d_tag) {
        case DT_INIT_ARRAY:
            m_initArray = displace<const InitFiniFunc>(entry->d_un.d_ptr);
            break;

        case DT_INIT_ARRAYSZ:
            m_initArraySize = entry->d_un.d_val;
            break;

        case DT_FINI_ARRAY:
            m_finiArray = displace<const InitFiniFunc>(entry->d_un.d_ptr);
            break;

        case DT_FINI_ARRAYSZ:
            m_finiArraySize = entry->d_un.d_val;
            break;

        case DT_HASH:
            m_hash = displace<const uint32_t>(entry->d_un.d_ptr);
            break;

        case DT_STRTAB:
            m_stringTable = displace<const char>(entry->d_un.d_ptr);
            break;

        case DT_SYMTAB:
            m_symbolTable = displace<const Elf64_Sym>(entry->d_un.d_ptr);
            break;

        case DT_STRSZ:
            m_stringTableSize = entry->d_un.d_val;
            break;

        case DT_SYMENT:
            if(entry->d_un.d_val != sizeof(Elf64_Sym))
                throw std::runtime_error("unexpected symbol table entry size");
            break;

        case DT_PLTGOT:
            m_got = displace(entry->d_un.d_ptr);
            break;

        case DT_PLTRELSZ:
            m_pltRelocationsSize = entry->d_un.d_val;
            break;

        case DT_PLTREL:
            if(entry->d_un.d_val != DT_RELA)
                throw std::runtime_error("unexpected type of the PLT relocations");
            break;

        case DT_JMPREL:
            m_pltRelocations = displace<const Elf64_Rela>(entry->d_un.d_ptr);
            break;

        case DT_RELA:
            m_relocations = displace<const Elf64_Rela>(entry->d_un.d_ptr);
            break;

        case DT_RELASZ:
            m_relocationsSize = entry->d_un.d_val;
            break;

        case DT_RELAENT:
            if(entry->d_un.d_val != sizeof(Elf64_Rela))
                throw std::runtime_error("unexpected size of a Rela relocation");
            break;

        default:

            fprintf(stderr, "the image has an unhandled .dynamic entry with the tag 0x%08" PRIx64 "\n", static_cast<uint64_t>(entry->d_tag));
        }
    }

    if(!m_hash)
        throw std::runtime_error("the image has no hash table");

    if(!m_stringTable)
        throw std::runtime_error("the image has no string table");

    if(!m_symbolTable)
        throw std::runtime_error("the image has no symbol table");

}

Image::~Image() = default;

bool Image::getSymbol(const char *name, void *&value) const {
    auto hash = symbolHash(name);

    auto nbucket = m_hash[0];
    auto nchain = m_hash[1];
    auto bucket = &m_hash[2];
    auto chain = &m_hash[2 + nbucket];

    for(size_t symbolEntry = bucket[hash % nbucket]; symbolEntry != 0; symbolEntry = chain[symbolEntry]) {
        const auto &symbol = m_symbolTable[symbolEntry];

        if(strcmp(name, m_stringTable + symbol.st_name) == 0) {
            if(symbol.st_shndx == SHN_UNDEF) {
                return false;
            }

            value = displace<void>(symbol.st_value);

            return true;
        }
    }

    return false;
}

uint32_t Image::symbolHash(const char *name) {
    auto uname = reinterpret_cast<const unsigned char *>(name);

    uint32_t hash = 0;
    while(*uname) {
        hash = (hash << 4) + *uname++;
        auto g = hash & UINT32_C(0xF0000000);

        if(g != 0) {
            hash ^= g >> 24;
        }

        hash &= ~g;
    }

    return hash;
}

void *Image::getSymbolChecked(const char *name) const {
    void *result;

    if(!getSymbol(name, result)) {
        fprintf(stderr, "Image::getSymbolChecked: a required symbol is not defined: \"%s\"\n", name);
        fflush(stderr);
        abort();
    }

    return result;
}

void Image::processRelocations(const Elf64_Rela *entries, size_t sizeBytes) {
    auto numberOfEntires = sizeBytes / sizeof(Elf64_Rela);
    auto limit = entries + numberOfEntires;

    for(auto entry = entries; entry < limit; entry++) {
        uint32_t type = ELF64_R_TYPE(entry->r_info);
        uint32_t symbol = ELF64_R_SYM(entry->r_info);

        auto target = displace<void *>(entry->r_offset);

        if(type == R_AARCH64_RELATIVE) {
            /*
             * RELATIVE relocs can be fast tracked here, since we never need
             * to resolve the symbol.
             */
            *target = displace(entry->r_addend);
        } else {
            auto resolved = resolveSymbol(symbol);

            if(type == R_AARCH64_ABS64 /* Direct 64-bit */ ||
               type == R_AARCH64_GLOB_DAT /* Create GOT entry. */ ||
               type == R_AARCH64_JUMP_SLOT /* Create PLT entry.  */
            ) {
                *target = static_cast<unsigned char *>(resolved) + entry->r_addend;
            } else {
                fprintf(stderr, "unhandled relocation type: %" PRId32 "\n", type);
            }
        }
    }
}

void *Image::resolveSymbol(uint32_t symbolIndex) {
    if(symbolIndex == STN_UNDEF)
        throw std::runtime_error("attempted to resolve the null symbol");

    if(symbolIndex >= m_hash[1])
        throw std::logic_error("the symbol index is out of range");

    const auto &symbol = m_symbolTable[symbolIndex];
    if(symbol.st_shndx == SHN_UNDEF) {
        auto name = m_stringTable + symbol.st_name;

        const Image *armlibToCheck = nullptr;

        auto &ctx = GlobalContext::get();
        if(ctx.hasArmlib()) {
            armlibToCheck = &ctx.armlib();
        }

        void *symbol;

        if(armlibToCheck && armlibToCheck->getSymbol(name, symbol)) {
            return symbol;
        } else {
            return resolveUndefinedARMSymbol(name);
        }

    } else {
        return displace(symbol.st_value);
    }
}

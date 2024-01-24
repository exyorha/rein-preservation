#include "Image.h"
#include "ElfModule.h"

#include <algorithm>
#include <elf.h>
#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <cstring>

#include <system_error>
#include <sys/mman.h>

#include "support.h"
#include "thunking.h"
#include "SystemAPIThunking.h"

std::shared_mutex Image::m_initializationMutex;
std::optional<Image> Image::m_armlib;
std::optional<Image> Image::m_il2cpp;
const uint32_t Image::PageSize = queryPageSize();

Image::Image(const std::filesystem::path &path) : m_module(ElfModule::createFromFile(path)),
    m_dynamic(nullptr),
    m_initArray(nullptr), m_initArraySize(0),
    m_finiArray(nullptr), m_finiArraySize(0),
    m_hash(nullptr),
    m_stringTable(nullptr), m_stringTableSize(0),
    m_symbolTable(nullptr),
    m_got(nullptr),
    m_relocations(nullptr), m_relocationsSize(0),
    m_pltRelocations(nullptr), m_pltRelocationsSize(0) {

    if(m_module->moduleClass() != ELFCLASS64 || m_module->machine() != EM_AARCH64) {
        throw std::runtime_error("unsupported module class or architecture");
    }

    createBackgroundMapping();
    mapImageSegments();

    printf("----- ARM image %s is occupying the memory range from %p to %p\n",
           path.c_str(),
           m_mapping->actualBase(), static_cast<unsigned char *>(m_mapping->actualBase()) + m_mapping->size());

    m_module.reset();

    parseDynamic();

    if(m_relocations && m_relocationsSize > 0)
        processRelocations(m_relocations, m_relocationsSize);

    if(m_pltRelocations && m_pltRelocationsSize > 0)
        processRelocations(m_pltRelocations, m_pltRelocationsSize);

    printf("init array: %p, %zu; fini array: %p, %zu\n",
           m_initArray, m_initArraySize,
           m_finiArray, m_finiArraySize);

    if(m_initArray && m_initArraySize > 0) {
        auto numberOfFunctions = m_initArraySize / sizeof(InitFiniFunc);
        for(auto func = m_initArray, limit = func + numberOfFunctions; func < limit; ++func) {
            printf("  init array: calling: %p\n", *func);
            armcall(*func);
        }
    }
}

void Image::createBackgroundMapping() {
    std::optional<uint64_t> lowestVA, highestVA;
    for(size_t entry = 0, num = m_module->phnum(); entry < num; entry++) {
        auto phdr = m_module->phent(entry);

        if(phdr.p_type == PT_LOAD) {
            if(!lowestVA.has_value() || *lowestVA > phdr.p_vaddr)
                lowestVA.emplace(phdr.p_vaddr);

            auto limit = phdr.p_vaddr + phdr.p_memsz;

            if(!highestVA.has_value() || *highestVA < limit)
                highestVA.emplace(limit);
        }
    }

    if(!lowestVA.has_value() || !highestVA.has_value()) {
        throw std::runtime_error("nothing to load in the image");
    }

    if(*lowestVA & (PageSize - 1))
        throw std::runtime_error("image lowest VA is not aligned to the system page size");

    auto imageSize = *highestVA - *lowestVA;

    imageSize = (imageSize + PageSize - 1) & ~(PageSize - 1);

    m_mapping.emplace(reinterpret_cast<void *>(*lowestVA), imageSize);
}

void Image::mapImageSegments() {
    for(size_t entry = 0, num = m_module->phnum(); entry < num; entry++) {
        auto phdr = m_module->phent(entry);

        if(phdr.p_type == PT_LOAD) {
            size_t mappedSize = 0;

            int prot = PROT_NONE;

            if(phdr.p_flags & PF_R)
                prot |= PROT_READ;

            if(phdr.p_flags & PF_W)
                prot |= PROT_WRITE;

            if(phdr.p_flags & PF_X)
                prot |= PROT_EXEC;

            if(phdr.p_filesz != 0) {
                if((phdr.p_offset & (PageSize - 1)) != (phdr.p_vaddr & (PageSize - 1)))
                    throw std::runtime_error("the program segment offset and the virtual address are not congruent modulo system page size");

                mappedSize = (phdr.p_filesz + (PageSize - 1)) & ~(PageSize - 1);

                auto misalign = phdr.p_vaddr & (PageSize - 1);

                auto alignedVaddr = phdr.p_vaddr & ~(PageSize - 1);
                auto alignedOffset = phdr.p_offset & ~(PageSize - 1);

                auto result = mmap(
                    displace(alignedVaddr), mappedSize,
                    prot,
                    MAP_PRIVATE | MAP_FIXED,
                    m_module->getFileDescriptor(),
                    alignedOffset);
                if(result == MAP_FAILED)
                    throw std::system_error(errno, std::generic_category());
            }

            if(phdr.p_memsz > mappedSize) {
                auto tailStart = (phdr.p_vaddr & ~(PageSize - 1)) + mappedSize;
                auto tailSize = (phdr.p_memsz - mappedSize + (PageSize - 1)) & ~(PageSize - 1);

                auto result = mprotect(displace(tailStart), tailSize, prot);
                if(result < 0) {
                    throw std::system_error(errno, std::generic_category());
                }
            }
        } else if(phdr.p_type == PT_DYNAMIC) {
            if(m_dynamic)
                throw std::runtime_error("the image has multiple dynamic headers");

            m_dynamic = displace<Elf64_Dyn>(phdr.p_vaddr);
        } else {
            fprintf(stderr, "The image has an unhandled PHDR entry of type 0x%08" PRIx32 "\n", phdr.p_type);
        }
    }
}

void Image::parseDynamic() {
    if(!m_dynamic)
        throw std::runtime_error("the image has no dynamic table");

    for(auto entry = m_dynamic; entry->d_tag != DT_NULL; ++entry) {
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

uint32_t Image::queryPageSize() {

    auto result = sysconf(_SC_PAGESIZE);
    if(result < 0) {
        throw std::system_error(errno, std::generic_category());
    }

    return static_cast<uint32_t>(result);
}

Image *Image::get_armlib_image() {
    Image *image = nullptr;

    {
        std::shared_lock<std::shared_mutex> shared(m_initializationMutex);

        if(m_armlib.has_value())
            image = &*m_armlib;
    }

    {
        std::unique_lock<std::shared_mutex> unique(m_initializationMutex);

        if(!m_armlib.has_value()) {
            setlinebuf(stdout);
            setlinebuf(stderr);

            auto path = thisLibraryDirectory() / "armlib.so";
            m_armlib.emplace(path);

            printf("---- armlib has been initialized ----\n");
        }

        image = &*m_armlib;
    }

    return image;
}

Image *Image::get_il2cpp_image() {
    Image *image = nullptr;

    /*
     * Ensure that armlib is loaded before il2cpp.
     */
    (void)get_armlib_image();

    {
        std::shared_lock<std::shared_mutex> shared(m_initializationMutex);

        if(m_il2cpp.has_value())
            image = &*m_il2cpp;
    }

    {
        std::unique_lock<std::shared_mutex> unique(m_initializationMutex);

        if(!m_il2cpp.has_value()) {
            auto path = thisLibraryDirectory() / "libil2cpp.so";
            m_il2cpp.emplace(path);

            printf("---- ARM il2cpp has been initialized, releasing normal execution ----\n");
        }

        image = &*m_il2cpp;
    }

    return image;
}

bool Image::getSymbol(const char *name, void *&value) const {
    auto hash = symbolHash(name);

    auto nbucket = m_hash[0];
    auto nchain = m_hash[1];
    auto bucket = &m_hash[2];
    auto chain = &m_hash[2 + nbucket];

    for(size_t symbolEntry = bucket[hash % nbucket]; symbolEntry != 0; symbolEntry = chain[symbolEntry]) {
        const auto &symbol = m_symbolTable[symbolEntry];

        if(strcmp(name, m_stringTable + symbol.st_name) == 0) {
            // If this assert triggers, then we need to look over the specs and evaluate whether
            // do we need to look over the rest of the link set for undefined symbols or not.

            if(symbol.st_shndx == SHN_UNDEF) {
                fprintf(stderr, "Image::getSymbol called for an undefined symbol '%s'\n", name);
                return false;
            }

            if(symbol.st_shndx == SHN_ABS) {
                value = reinterpret_cast<void *>(static_cast<uintptr_t>(symbol.st_value));
            } else {
                value = displace<void>(symbol.st_value);
            }

            return true;
        }
    }

    return false;


    printf("Image::getSymbol(%s) not implemented!\n", name);
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
    if(symbol.st_shndx == SHN_ABS) {
        return reinterpret_cast<void *>(symbol.st_value);
    } else if(symbol.st_shndx == SHN_UNDEF) {
        auto name = m_stringTable + symbol.st_name;

        Image *armlibToCheck = nullptr;
        if(m_armlib.has_value() && &*m_armlib != this) {
            armlibToCheck = &*m_armlib;
        }

        void *symbol;

        if(armlibToCheck && armlibToCheck->getSymbol(name, symbol)) {
            printf("resolved %s to %p from armlib\n", name, symbol);
            return symbol;
        } else {
            return resolveUndefinedARMSymbol(name);
        }

    } else {
        return displace(symbol.st_value);
    }
}

Image::ImageMapping::ImageMapping(void *preferredBase, size_t size) : m_preferredBase(preferredBase), m_size(size) {
    m_base = mmap(preferredBase, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(m_base == MAP_FAILED)
        throw std::system_error(errno, std::generic_category());
}

Image::ImageMapping::~ImageMapping() {
    munmap(m_base, m_size);
}

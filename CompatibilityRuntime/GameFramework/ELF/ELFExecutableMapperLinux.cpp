#include <ELF/ELFExecutableMapperLinux.h>
#include <ELF/ElfModule.h>

#if defined(_WIN32)
#include <windows.h>
#include <Windows/WindowsError.h>
#else
#include <sys/mman.h>
#endif

#include <cinttypes>

#include <sys/mman.h>

#include "GlobalContext.h"

ELFExecutableMapperLinux::ELFExecutableMapperLinux(const std::filesystem::path &path) :
    m_module(ElfModule::createFromFile(path)), m_phdr(nullptr), m_dynamic(nullptr), m_phnum(0) {

    if(m_module->moduleClass() != ELFCLASS64 || m_module->machine() != EM_AARCH64) {
        throw std::runtime_error("unsupported module class or architecture");
    }

    m_phnum = m_module->phnum();

    createBackgroundMapping();
    mapImageSegments();

    if(!m_phdr)
        throw std::runtime_error("unable to map the PHDR into one of the segments");

    printf("----- ARM image %s is occupying the memory range from %p to %p\n",
           path.string().c_str(),
           m_mapping->actualBase(), static_cast<unsigned char *>(m_mapping->actualBase()) + m_mapping->size());

    m_module.reset();
}

ELFExecutableMapperLinux::~ELFExecutableMapperLinux() = default;

void ELFExecutableMapperLinux::createBackgroundMapping() {
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

    if(*lowestVA & (GlobalContext::PageSize - 1))
        throw std::runtime_error("image lowest VA is not aligned to the system page size");

    auto imageSize = *highestVA - *lowestVA;

    imageSize = (imageSize + GlobalContext::PageSize - 1) & ~(GlobalContext::PageSize - 1);

    m_mapping.emplace(reinterpret_cast<void *>(*lowestVA), imageSize);
}

void ELFExecutableMapperLinux::mapImageSegments() {
    auto phstart = m_module->phoff();
    auto phend = phstart + m_module->phnum() * sizeof(Elf64_Phdr);

    for(size_t entry = 0, num = m_module->phnum(); entry < num; entry++) {
        auto phdr = m_module->phent(entry);

        if(phdr.p_type == PT_LOAD) {
            uintptr_t endOfMappedFileContents = phdr.p_vaddr;
            uintptr_t endOfZeroedFileContents = phdr.p_vaddr + phdr.p_memsz;

            int prot = PROT_NONE;

            if(phdr.p_flags & PF_R)
                prot |= PROT_READ;

            if(phdr.p_flags & PF_W)
                prot |= PROT_WRITE;

            if(phdr.p_flags & PF_X)
                prot |= PROT_EXEC;

            if(phdr.p_filesz != 0) {
                if((phdr.p_offset & (GlobalContext::PageSize - 1)) != (phdr.p_vaddr & (GlobalContext::PageSize - 1)))
                    throw std::runtime_error("the program segment offset and the virtual address are not congruent modulo system page size");

                endOfMappedFileContents = (phdr.p_vaddr + phdr.p_filesz + GlobalContext::PageSize - 1) & ~(GlobalContext::PageSize - 1);

                auto misalign = phdr.p_vaddr & (GlobalContext::PageSize - 1);

                auto alignedVaddr = phdr.p_vaddr & ~(GlobalContext::PageSize - 1);
                auto alignedOffset = phdr.p_offset & ~(GlobalContext::PageSize - 1);

                auto result = mmap(
                    displace(alignedVaddr), endOfMappedFileContents - alignedVaddr,
                    prot,
                    MAP_PRIVATE | MAP_FIXED,
                    m_module->getFileDescriptor(),
                    alignedOffset + m_module->elfBodyOffset());
                if(result == MAP_FAILED)
                    throw std::system_error(errno, std::generic_category());
            }

            if(endOfZeroedFileContents > endOfMappedFileContents) {
                auto tailSize = (endOfZeroedFileContents - endOfMappedFileContents + (GlobalContext::PageSize - 1))  & ~(GlobalContext::PageSize - 1);

                auto result = mprotect(displace(endOfMappedFileContents), tailSize, prot);
                if(result < 0) {
                    throw std::system_error(errno, std::generic_category());
                }
            }

            if(phdr.p_memsz > phdr.p_filesz) {
                memset(displace<char>(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
            }

            if(!m_phdr && (phdr.p_offset <= phstart && phdr.p_offset + phdr.p_filesz >= phend)) {
                m_phdr = displace<Elf64_Phdr>(phdr.p_vaddr + (phstart - phdr.p_offset));
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

ELFExecutableMapperLinux::ImageMapping::ImageMapping(void *preferredBase, size_t size) : m_preferredBase(preferredBase), m_size(size) {
    m_base = mmap(preferredBase, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(m_base == MAP_FAILED)
        throw std::system_error(errno, std::generic_category());
}

ELFExecutableMapperLinux::ImageMapping::~ImageMapping() {
    munmap(m_base, m_size);
}


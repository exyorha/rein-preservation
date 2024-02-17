#include <ELF/ElfModule.h>
#include <PETypes.h>
#include <ELFPETypes.h>

#include "FileDescriptor.h"

#include <fcntl.h>
#include <string.h>

#include <system_error>
#include <type_traits>

ElfModule::ElfModule() = default;

ElfModule::~ElfModule() = default;

void elfCheckedRead(const ElfHandleType &handle, void *dest, size_t size, off_t offset) {
    auto result = pread(handle, dest, size, offset);
    if(result < 0)
        throw std::system_error(errno, std::generic_category());

    if(result != size)
        throw std::logic_error("short read reading the ELF file data");
}

std::unique_ptr<ElfModule> ElfModule::createFromFile(const std::filesystem::path &filename) {
    int rawFd = open(filename.c_str(), O_RDONLY | O_CLOEXEC | O_NOCTTY);
    if(rawFd < 0)
        throw std::system_error(errno, std::generic_category());

    FileDescriptor fd(rawFd);

    unsigned char ident[EI_NIDENT];
    elfCheckedRead(fd, ident, sizeof(ident), 0);

    size_t elfBodyOffset = 0;

    if(ident[0] == 'M' && ident[1] == 'Z') {
        static_assert(sizeof(IMAGE_DOS_HEADER) > sizeof(ident));

        IMAGE_DOS_HEADER dosHeader;
        memcpy(&dosHeader, ident, sizeof(ident));

        int elfPEOffset = 512 * dosHeader.e_cp;
        if(dosHeader.e_cblp != 0) {
            elfPEOffset = elfPEOffset - 512 + dosHeader.e_cblp;
        }

        if(elfPEOffset < 0)
            throw std::logic_error("bad e_cblp");

        ELFPEHeader elfpe;
        elfCheckedRead(fd, &elfpe, sizeof(elfpe), elfPEOffset);
        if(elfpe.signature != ELFPEHeader::ELFPESignature) {
            throw std::runtime_error("MZ file at input, but no ELF-PE signature");
        }

        elfBodyOffset = elfpe.elfOffset;

        elfCheckedRead(fd, ident, sizeof(ident), elfBodyOffset);
    }

    if(ident[EI_MAG0] != ELFMAG0 ||
        ident[EI_MAG1] != ELFMAG1 ||
        ident[EI_MAG2] != ELFMAG2 ||
        ident[EI_MAG3] != ELFMAG3)
        throw std::logic_error("bad ELF magic");

    auto elfClass = ident[EI_CLASS];

    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big,
                  "Host platform must be either big or little endian");

    if((std::endian::native == std::endian::little && ident[EI_DATA] != ELFDATA2LSB) ||
       (std::endian::native == std::endian::big && ident[EI_DATA] != ELFDATA2MSB))
        throw std::logic_error("unsupported ELF byte order");

    if(ident[EI_VERSION] != EV_CURRENT)
        throw std::logic_error("unsupported ELF version");

    /*
     * We don't check ABI, since that could be all over the place.
     */

    for(size_t byte = EI_PAD; byte < sizeof(ident); byte++) {
        if(ident[byte] != 0) {
            throw std::logic_error("non-zero padding in ei_ident");
        }
    }

    if(elfClass == ELFCLASS32) {
        return std::make_unique<ElfModuleImpl<uint32_t>>(ident, std::move(fd), elfBodyOffset);
    } else if(elfClass == ELFCLASS64) {
        return std::make_unique<ElfModuleImpl<uint64_t>>(ident, std::move(fd), elfBodyOffset);
    } else {
        throw std::logic_error("unsupported ELF class");
    }
}

template<typename T>
ElfModuleImpl<T>::ElfModuleImpl(const unsigned char *ident, ElfHandleType &&handle, size_t elfBodyOffset) :
    m_handle(std::move(handle)), m_elfBodyOffset(elfBodyOffset) {
    static_assert(offsetof(decltype(m_ehdr), e_ident) == 0, "Ehdr should start with e_ident");

    memcpy(m_ehdr.e_ident, ident, EI_NIDENT);

    readFileData(reinterpret_cast<unsigned char *>(&m_ehdr) + EI_NIDENT, sizeof(m_ehdr) - EI_NIDENT,
                 EI_NIDENT);

    if(m_ehdr.e_version != EV_CURRENT)
        throw std::logic_error("unsupported ELF version");

    if(m_ehdr.e_type != ET_DYN)
        throw std::logic_error("unsupported executable type");

    if(m_ehdr.e_phentsize != sizeof(Phdr))
        throw std::logic_error("unexpected e_phentsize");

    m_phdr.resize(m_ehdr.e_phnum);

    readFileData(m_phdr.data(), m_phdr.size() * sizeof(Phdr), m_ehdr.e_phoff);
}

template<typename T>
void ElfModuleImpl<T>::readFileData(void *data, size_t size, off_t offset) {
    elfCheckedRead(m_handle, data, size, offset + m_elfBodyOffset);
}

template<typename T>
ElfModuleImpl<T>::~ElfModuleImpl() = default;

template<typename T>
uint8_t ElfModuleImpl<T>::moduleClass() const {
    return m_ehdr.e_ident[EI_CLASS];
}

template<typename T>
uint16_t ElfModuleImpl<T>::machine() const {
    return m_ehdr.e_machine;
}

template<typename T>
uint64_t ElfModuleImpl<T>::entryPoint() const {
    return m_ehdr.e_entry;
}

template<typename T>
size_t ElfModuleImpl<T>::phnum() const {
    return m_phdr.size();
}

template<typename T>
size_t ElfModuleImpl<T>::phoff() const {
    return m_ehdr.e_phoff;
}

template<typename T>
Elf64_Phdr ElfModuleImpl<T>::phent(size_t index) const {
    return Traits::normalizePHDR(m_phdr.at(index));
}

template<typename T>
int ElfModuleImpl<T>::getFileDescriptor() const {
    return m_handle;
}

template<typename T>
size_t ElfModuleImpl<T>::elfBodyOffset() const {
    return m_elfBodyOffset;
}

template class ElfModuleImpl<uint32_t>;
template class ElfModuleImpl<uint64_t>;

Elf64_Phdr ElfClassTraits<uint32_t>::normalizePHDR(const Elf32_Phdr &in) {
    Elf64_Phdr out = {
        .p_type = in.p_type,
        .p_flags = in.p_flags,
        .p_offset = in.p_offset,
        .p_vaddr = in.p_vaddr,
        .p_paddr = in.p_paddr,
        .p_filesz = in.p_filesz,
        .p_memsz = in.p_memsz,
        .p_align = in.p_align
    };

    return out;
}

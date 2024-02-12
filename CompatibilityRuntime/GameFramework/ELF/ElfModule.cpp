#include <ELF/ElfModule.h>
#include "FileDescriptor.h"

#include <fcntl.h>
#include <string.h>

#include <system_error>
#include <type_traits>

#if defined(_WIN32)
#include <Windows/WindowsError.h>
#endif


ElfModule::ElfModule() = default;

ElfModule::~ElfModule() = default;

void elfCheckedRead(const ElfHandleType &handle, void *dest, size_t size) {
#if defined(_WIN32)
    DWORD bytesRead;
    if(!ReadFile(handle.get(), dest, static_cast<DWORD>(size), &bytesRead, nullptr))
        WindowsError::throwLastError();

    if(bytesRead != size)
        throw std::runtime_error("short read");

#else
    auto result = read(handle, dest, size);
    if(result < 0)
        throw std::system_error(errno, std::generic_category());

    if(result != size)
        throw std::runtime_error("short read");
#endif
}

std::unique_ptr<ElfModule> ElfModule::createFromFile(const std::filesystem::path &filename) {
#if defined(_WIN32)

    auto rawHandle = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if(rawHandle == INVALID_HANDLE_VALUE)
        WindowsError::throwLastError();

    WindowsHandle fd(rawHandle);
#else
    int rawFd = open(filename.c_str(), O_RDONLY | O_CLOEXEC | O_NOCTTY);
    if(rawFd < 0)
        throw std::system_error(errno, std::generic_category());

    FileDescriptor fd(rawFd);
#endif

    unsigned char ident[EI_NIDENT];
    elfCheckedRead(fd, ident, sizeof(ident));

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
        return std::make_unique<ElfModuleImpl<uint32_t>>(ident, std::move(fd));
    } else if(elfClass == ELFCLASS64) {
        return std::make_unique<ElfModuleImpl<uint64_t>>(ident, std::move(fd));
    } else {
        throw std::logic_error("unsupported ELF class");
    }
}

template<typename T>
ElfModuleImpl<T>::ElfModuleImpl(const unsigned char *ident, ElfHandleType &&handle) : m_handle(std::move(handle)) {
    static_assert(offsetof(decltype(m_ehdr), e_ident) == 0, "Ehdr should start with e_ident");

    memcpy(m_ehdr.e_ident, ident, EI_NIDENT);

    elfCheckedRead(m_handle, reinterpret_cast<unsigned char *>(&m_ehdr) + EI_NIDENT, sizeof(m_ehdr) - EI_NIDENT);

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
#if defined(_WIN32)
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));

    auto offset64 = static_cast<uint64_t>(offset);

    overlapped.Offset = static_cast<DWORD>(offset64);
    overlapped.OffsetHigh = static_cast<DWORD>(offset64 >> 32);

    DWORD bytesRead;
    if(!ReadFile(m_handle.get(), data, static_cast<DWORD>(size), &bytesRead, &overlapped))
        WindowsError::throwLastError();

    if(bytesRead != size)
        throw std::runtime_error("short read");

#else
    auto result = pread(m_handle, data, size, offset);
    if(result < 0)
        throw std::system_error(errno, std::generic_category());

    if(result != size)
        throw std::logic_error("short read reading the ELF file data");
#endif
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

#ifndef _WIN32
template<typename T>
int ElfModuleImpl<T>::getFileDescriptor() const {
    return m_handle;
}
#endif

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

#ifndef ELF_ELF_EXECUTABLE_MAPPER_WINDOWS_H
#define ELF_ELF_EXECUTABLE_MAPPER_WINDOWS_H

#include <windows.h>

#include <filesystem>

#include <musl-elf.h>

class ELFExecutableMapperWindows {
public:
    explicit ELFExecutableMapperWindows(const std::filesystem::path &filename);
    ~ELFExecutableMapperWindows();

    ELFExecutableMapperWindows(const ELFExecutableMapperWindows &other) = delete;
    ELFExecutableMapperWindows &operator =(const ELFExecutableMapperWindows &other) = delete;

    const Elf64_Phdr* phdr() const {
        return m_phdr;
    }

    size_t phnum() const {
        return m_phnum;
    }

    const Elf64_Dyn *dynamic() const {
        return m_dynamic;
    }

    intptr_t displacement() const {
        return m_displacement;
    }

private:
    template<typename T>
    const T *rva(uint32_t rva) {
        return reinterpret_cast<const T *>(static_cast<char *>(m_image.base()) + rva);
    }

    template<typename T = void>
    inline T *displace(uintptr_t address) const {
        return reinterpret_cast<T *>(address + displacement());
    }

    class ImageResource {
    public:
        explicit ImageResource(const std::filesystem::path &filename);
        ~ImageResource();

        ImageResource(const ImageResource &other) = delete;
        ImageResource &operator =(const ImageResource &other) = delete;

        inline void *base() const {
            return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(m_module) & ~static_cast<uintptr_t>(65535));
        }

    private:
        HMODULE m_module;
    };

    ImageResource m_image;
    intptr_t m_displacement;
    const Elf64_Phdr *m_phdr;
    size_t m_phnum;
    const Elf64_Dyn *m_dynamic;
};

#endif

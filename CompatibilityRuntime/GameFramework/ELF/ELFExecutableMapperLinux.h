#ifndef ELF_ELF_EXECUTABLE_MAPPER_LINUX_H
#define ELF_ELF_EXECUTABLE_MAPPER_LINUX_H

#include <musl-elf.h>

#include <filesystem>
#include <memory>
#include <optional>

class ElfModule;

class ELFExecutableMapperLinux {
public:
    explicit ELFExecutableMapperLinux(const std::filesystem::path &filename);
    ~ELFExecutableMapperLinux();

    ELFExecutableMapperLinux(const ELFExecutableMapperLinux &other) = delete;
    ELFExecutableMapperLinux &operator =(const ELFExecutableMapperLinux &other) = delete;

    inline const Elf64_Phdr* phdr() const {
        return m_phdr;
    }

    inline size_t phnum() const {
        return m_phnum;
    }

    inline const Elf64_Dyn *dynamic() const {
        return m_dynamic;
    }

    inline intptr_t displacement() const {
        return m_mapping->displacement();
    }

private:
    class ImageMapping {
    public:
        explicit ImageMapping(void *preferredBase, size_t size);
        ~ImageMapping();

        ImageMapping(const ImageMapping &other) = delete;
        ImageMapping &operator =(const ImageMapping &other) = delete;

        inline void *preferredBase() const {
            return m_preferredBase;
        }

        inline void *actualBase() const {
            return m_base;
        }

        inline intptr_t displacement() const {
            return reinterpret_cast<uintptr_t>(m_base) - reinterpret_cast<uintptr_t>(m_preferredBase);
        }

        inline size_t size() const {
            return m_size;
        }

    private:
        void *m_preferredBase;
        void *m_base;
        size_t m_size;
    };

    void createBackgroundMapping();
    void mapImageSegments();

    template<typename T = void>
    inline T *displace(uintptr_t address) const {
        return reinterpret_cast<T *>(address + m_mapping->displacement());
    }

    std::unique_ptr<ElfModule> m_module;
    const Elf64_Phdr *m_phdr;
    const Elf64_Dyn *m_dynamic;
    size_t m_phnum;
    std::optional<ImageMapping> m_mapping;
};

#endif

#ifndef IMAGE_H
#define IMAGE_H

#include <shared_mutex>
#include <optional>
#include <filesystem>
#include <memory>

#include <elf.h>

class ElfModule;

class Image {
public:
    explicit Image(const std::filesystem::path &path);
    ~Image();

    Image(const Image &other) = delete;
    Image &operator =(const Image &other) = delete;

    inline const std::filesystem::path &path() const {
        return m_path;
    }

    bool getSymbol(const char *name, void *&symbol) const;
    void *getSymbolChecked(const char *name) const;

    inline intptr_t displacement() const {
        return m_mapping->displacement();
    }

    inline const Elf64_Phdr* phdr() const {
        return m_phdr;
    }

    inline size_t phnum() const {
        return m_phnum;
    }

private:
    void createBackgroundMapping();
    void mapImageSegments();
    void parseDynamic();
    void processRelocations(const Elf64_Rela *entries, size_t sizeBytes);
    void *resolveSymbol(uint32_t symbolIndex);

    static uint32_t symbolHash(const char *name);

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

    template<typename T = void>
    inline T *displace(uintptr_t address) const {
        return reinterpret_cast<T *>(address + m_mapping->displacement());
    }

    using InitFiniFunc = void (*)();

    std::filesystem::path m_path;
    std::unique_ptr<ElfModule> m_module;
    std::optional<ImageMapping> m_mapping;
    const Elf64_Dyn *m_dynamic;
    const InitFiniFunc *m_initArray;
    size_t m_initArraySize;
    const InitFiniFunc *m_finiArray;
    size_t m_finiArraySize;
    const uint32_t *m_hash;
    const char *m_stringTable;
    size_t m_stringTableSize;
    const Elf64_Sym *m_symbolTable;
    void *m_got;
    const Elf64_Rela *m_relocations;
    size_t m_relocationsSize;
    const Elf64_Rela *m_pltRelocations;
    size_t m_pltRelocationsSize;
    Elf64_Phdr *m_phdr;
    size_t m_phnum;
};

#endif

#ifndef IMAGE_H
#define IMAGE_H


#include <shared_mutex>
#include <optional>
#include <filesystem>
#include <memory>

#include <musl-elf.h>

#include <ELF/ELFExecutableMapper.h>
#include <ELF/ElfSymbolSource.h>

class Image final : public ElfSymbolSource {
public:
    Image(const ElfSymbolSource *symbolSource, const std::filesystem::path &path);
    ~Image();

    Image(const Image &other) = delete;
    Image &operator =(const Image &other) = delete;

    inline const std::filesystem::path &path() const {
        return m_path;
    }

    inline const Elf64_Phdr *phdr() const {
        return m_mapper.phdr();
    }

    inline size_t phnum() const {
        return m_mapper.phnum();
    }

    bool lookup(const char *name, void *&value, bool *isWeak) const override;

    inline intptr_t displacement() const {
        return m_mapper.displacement();
    }

    void runConstructors();

    template<typename T = void>
    inline T *displace(uintptr_t address) const {
        return reinterpret_cast<T *>(address + displacement());
    }

    void bind();

private:
    void parseDynamic();
    void processRelocations(const Elf64_Rela *entries, size_t sizeBytes);
    void *resolveSymbol(uint32_t symbolIndex);

    static uint32_t symbolHash(const char *name);

    using InitFiniFunc = void (*)();

    const ElfSymbolSource *m_symbolSource;
    ELFExecutableMapper m_mapper;
    std::filesystem::path m_path;
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
    bool m_symbolic;
};

#endif

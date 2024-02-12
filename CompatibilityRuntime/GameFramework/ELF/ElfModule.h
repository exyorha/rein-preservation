#ifndef ELF_MODULE_H
#define ELF_MODULE_H

#include <memory>
#include <filesystem>
#include <vector>

#include <ELF/musl-elf.h>

#ifndef _WIN32
#include "FileDescriptor.h"
#endif

class ElfModule {
protected:
    ElfModule();

public:
    virtual ~ElfModule();

    ElfModule(const ElfModule &other) = delete;
    ElfModule &operator =(const ElfModule &other) = delete;

    virtual uint8_t moduleClass() const = 0;
    virtual uint16_t machine() const = 0;
    virtual uint64_t entryPoint() const = 0;

    virtual size_t phnum() const = 0;
    virtual size_t phoff() const = 0;
    virtual Elf64_Phdr phent(size_t index) const = 0;

    virtual void readFileData(void *data, size_t size, off_t offset) = 0;

#ifndef _WIN32
    virtual int getFileDescriptor() const = 0;
#endif

    static std::unique_ptr<ElfModule> createFromFile(const std::filesystem::path &filename);
};

template<typename T>
struct ElfClassTraits {

};

template<>
struct ElfClassTraits<uint32_t> {
    using Ehdr = Elf32_Ehdr;
    using Phdr = Elf32_Phdr;
    using Sym = Elf32_Sym;
    using Rel = Elf32_Rel;
    using Rela = Elf32_Rela;

    static Elf64_Phdr normalizePHDR(const Elf32_Phdr &in);
};

template<>
struct ElfClassTraits<uint64_t> {
    using Ehdr = Elf64_Ehdr;
    using Phdr = Elf64_Phdr;
    using Sym = Elf64_Sym;
    using Rel = Elf64_Rel;
    using Rela = Elf64_Rela;

    static inline const Elf64_Phdr &normalizePHDR(const Elf64_Phdr &in) {
        return in;
    }
};

template<typename T>
class ElfModuleImpl final : public ElfModule {
public:
    using Traits = ElfClassTraits<T>;
    using Ehdr = Traits::Ehdr;
    using Phdr = Traits::Phdr;

#ifdef _WIN32
#else
    ElfModuleImpl(const unsigned char *ident, FileDescriptor &&fd);
#endif
    ~ElfModuleImpl() override;

    uint8_t moduleClass() const override;
    uint16_t machine() const override;
    uint64_t entryPoint() const override;

    size_t phnum() const override;
    size_t phoff() const override;
    Elf64_Phdr phent(size_t index) const override;

    void readFileData(void *data, size_t size, off_t offset) override;
#ifndef _WIN32
    int getFileDescriptor() const override;
#endif

private:
    Ehdr m_ehdr;
    std::vector<Phdr> m_phdr;
#ifndef _WIN32
    FileDescriptor m_fd;
#endif
};

#endif

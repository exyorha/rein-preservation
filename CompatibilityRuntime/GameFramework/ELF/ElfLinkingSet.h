#ifndef ELF_ELF_LINKING_SET_H
#define ELF_ELF_LINKING_SET_H

#include <vector>
#include <memory>
#include <filesystem>

#include <ELF/ElfSymbolSource.h>

class Image;

class ElfLinkingSet final : public ElfSymbolSource {
public:
    explicit ElfLinkingSet(ElfSymbolSource *fallbackSymbolSource = nullptr);
    ~ElfLinkingSet();

    ElfLinkingSet(const ElfLinkingSet &other) = delete;
    ElfLinkingSet &operator =(const ElfLinkingSet &other) = delete;

    const Image *loadExecutable(const std::filesystem::path &path);

    void bind();

    void runConstructors();

    bool lookup(const char *name, void *&value, bool *isWeak = nullptr) const override;

    const Image *mainImage() const;

    inline const std::vector<std::unique_ptr<Image>> &images() const {
        return m_images;
    }

private:
    ElfSymbolSource *m_fallbackSymbolSource;
    std::vector<std::unique_ptr<Image>> m_images;

};

#endif


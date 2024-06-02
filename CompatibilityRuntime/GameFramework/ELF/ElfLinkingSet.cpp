#include <ELF/ElfLinkingSet.h>
#include <ELF/Image.h>

ElfLinkingSet::ElfLinkingSet(ElfSymbolSource *fallbackSymbolSource) : m_fallbackSymbolSource(fallbackSymbolSource) {

}

ElfLinkingSet::~ElfLinkingSet() = default;

const Image *ElfLinkingSet::loadExecutable(const std::filesystem::path &path) {
    return m_images.emplace_back(std::make_unique<Image>(this, path)).get();
}

void ElfLinkingSet::bind() {
    for(const auto &image: m_images) {
        image->bind();
    }
}

void ElfLinkingSet::runConstructors() {
    for(const auto &image: m_images) {
        image->runConstructors();
    }
}

const Image *ElfLinkingSet::mainImage() const {
    if(m_images.empty())
        throw std::logic_error("no images loaded");

    return m_images.back().get();
}

bool ElfLinkingSet::lookup(const char *name, void *&value, bool *isWeak) const {
    std::optional<void *> foundWeak;

    for(const auto &image: m_images) {
        bool weak;

        if(image->lookup(name, value, &weak)) {
            if(weak && foundWeak.has_value()) {
                foundWeak.emplace(value);
            }
            if(isWeak)
                *isWeak = false;
            return true;
        }
    }

    if(foundWeak.has_value()) {

        if(isWeak)
            *isWeak = true;

        value = *foundWeak;

        return true;
    }

    if(m_fallbackSymbolSource) {
        return m_fallbackSymbolSource->lookup(name, value, isWeak);
    } else {
        return false;
    }
}


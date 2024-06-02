#include <ELF/ElfSymbolSource.h>
#include "support.h"

ElfSymbolSource::ElfSymbolSource() = default;

ElfSymbolSource::~ElfSymbolSource() = default;

void *ElfSymbolSource::getSymbolChecked(const char *name, bool *isWeak) const {
    void *result;

    if(!lookup(name, result, isWeak)) {
        panic("Image::getSymbolChecked: a required symbol is not defined: \"%s\"\n", name);
    }

    return result;
}


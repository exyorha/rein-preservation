#ifndef THUNK_SYMBOL_SOURCE_H
#define THUNK_SYMBOL_SOURCE_H

#include <ELF/ElfSymbolSource.h>

class ThunkSymbolSource final : public ElfSymbolSource {
public:
    ThunkSymbolSource();
    ~ThunkSymbolSource();

    bool lookup(const char *name, void *&value, bool *isWeak) const override;
};

#endif

#ifndef ELF_ELF_SYMBOL_SOURCE_H
#define ELF_ELF_SYMBOL_SOURCE_H

class ElfSymbolSource {
protected:
    ElfSymbolSource();
    ~ElfSymbolSource();

public:
    ElfSymbolSource(const ElfSymbolSource &other) = delete;
    ElfSymbolSource &operator =(const ElfSymbolSource &other) = delete;

    virtual bool lookup(const char *name, void *&value, bool *isWeak = nullptr) const = 0;

    void *getSymbolChecked(const char *name, bool *isWeak = nullptr) const;
};

#endif

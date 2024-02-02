#ifndef GAME_FRAMEWORK_TRANSLATOR_DIVERSION_MANAGER_H
#define GAME_FRAMEWORK_TRANSLATOR_DIVERSION_MANAGER_H

#include <shared_mutex>
#include <unordered_map>

#include <Translator/BumpAllocator.h>

struct Diversion;

using DiversionHandler = void (*)(const Diversion *diversion);

struct Diversion {
    uint32_t code[4];
    uintptr_t continueAddress;
    DiversionHandler handler;
    void *userdata;

    inline void invoke() const {
        handler(this);
    }

    inline uintptr_t continueThunkAddress() const {
        return reinterpret_cast<uintptr_t>(this);
    }
};

class DiversionManager {
public:

    DiversionManager();
    ~DiversionManager();

    DiversionManager(const DiversionManager &other) = delete;
    DiversionManager &operator =(const DiversionManager &other) = delete;

    const Diversion *divert(void *armCodePointer, DiversionHandler handler, void *userdata);

    inline bool isDivertedAddress(uintptr_t armCodePointer) {
        return getDiversion(reinterpret_cast<void *>(armCodePointer)) != nullptr;
    }

    const Diversion *getDiversion(void *armCodePointer);

private:

    std::shared_mutex m_diversionTableMutex;
    std::unordered_map<void *, const Diversion *> m_diversionTable;
    BumpAllocator m_diversionAllocator;
};

#endif


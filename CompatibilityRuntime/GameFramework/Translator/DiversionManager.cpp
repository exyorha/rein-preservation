#include <Translator/DiversionManager.h>

#include <mutex>

#include "support.h"
#include "GlobalContext.h"

DiversionManager::DiversionManager() = default;

DiversionManager::~DiversionManager() = default;

const Diversion *DiversionManager::divert(void *armCodePointer, DiversionHandler handler, void *userdata) {
    auto sourceCodeLocation = static_cast<const uint32_t *>(armCodePointer);

    auto diversion = static_cast<Diversion *>(m_diversionAllocator.allocate(sizeof(Diversion)));
    diversion->code[0] = *sourceCodeLocation;
    diversion->code[1] = UINT32_C(0x58000070); // LDR x16, tailAddress
    diversion->code[2] = UINT32_C(0xD61F0200); // BR x16
    diversion->code[3] = 0;
    diversion->continueAddress = reinterpret_cast<uintptr_t>(sourceCodeLocation + 1);
    diversion->handler = handler;
    diversion->userdata = userdata;

    {
        std::unique_lock<std::shared_mutex> locker(m_diversionTableMutex);

        auto it = m_diversionTable.emplace(armCodePointer, diversion);

        if(!it.second) {
            panic("attempted to divert an already-diverted ARM address: %p\n", armCodePointer);
        }
    }

    GlobalContext::get().jit().flushInstructionCache(reinterpret_cast<uintptr_t>(armCodePointer), 4);

    return diversion;
}

const Diversion * DiversionManager::getDiversion(void *armCodePointer) {
    std::shared_lock<std::shared_mutex> locker(m_diversionTableMutex);

    auto it = m_diversionTable.find(armCodePointer);
    if(it == m_diversionTable.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}

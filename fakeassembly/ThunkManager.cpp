#include "ThunkManager.h"

#include <sys/mman.h>

#include <system_error>
#include <mutex>


std::shared_mutex ThunkManager::m_thunkTableMutex;
std::unordered_map<ThunkManager::X86ThunkTarget, void *> ThunkManager::m_thunkX86ToArmTableForward;
std::unordered_map<void *, ThunkManager::X86ThunkTarget> ThunkManager::m_thunkX86ToArmTableReverse;
void *ThunkManager::m_currentARMThunkBlock = nullptr;
size_t ThunkManager::m_currentARMThunkBlockRemaining = 0;

void *ThunkManager::allocateARMToX86ThunkCall(X86ThunkTarget x86SideTarget) {
    if(!x86SideTarget)
        throw std::logic_error("cannot thunk to null");

    {
        std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkX86ToArmTableForward.find(x86SideTarget);
        if(it != m_thunkX86ToArmTableForward.end()) {
            return it->second;
        }
    }

    {
        std::unique_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkX86ToArmTableForward.find(x86SideTarget);
        if(it != m_thunkX86ToArmTableForward.end()) {
            return it->second;
        }

        auto address = allocateARMThunkAddressLocked();
        m_thunkX86ToArmTableForward.emplace(x86SideTarget, address);
        m_thunkX86ToArmTableReverse.emplace(address, x86SideTarget);

        printf("new thunk allocated: ARM call to %p will be translated to an X86 call to %p\n",
               address, x86SideTarget);

        return address;
    }
}

ThunkManager::X86ThunkTarget ThunkManager::lookupARMToX86ThunkCall(void *armCallAddress) {
    std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

    auto it = m_thunkX86ToArmTableReverse.find(armCallAddress);

    if(it != m_thunkX86ToArmTableReverse.end())
        return it->second;

    return nullptr;
}

void *ThunkManager::allocateARMThunkAddressLocked() {
    printf("allocating new ARM thunk, block: %p, remaining: %zu\n", m_currentARMThunkBlock, m_currentARMThunkBlockRemaining);

    if(m_currentARMThunkBlockRemaining == 0) {
        m_currentARMThunkBlock = mmap(nullptr, ThunkBlockAllocationSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(m_currentARMThunkBlock == MAP_FAILED)
            throw std::system_error(errno, std::generic_category());

        auto fillPtr = static_cast<uint32_t *>(m_currentARMThunkBlock);
        auto fillLimit = fillPtr + ThunkBlockAllocationSize / sizeof(uint32_t);

        for(auto ptr = fillPtr; ptr < fillLimit; ptr++) {
            *ptr = UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMToX86ThunkCallSVC) << 5);
        }

        if(mprotect(m_currentARMThunkBlock, ThunkBlockAllocationSize, PROT_READ | PROT_EXEC) < 0)
            throw std::system_error(errno, std::generic_category());

        m_currentARMThunkBlockRemaining = ThunkBlockAllocationSize;
    }

    auto ptr = static_cast<uint8_t *>(m_currentARMThunkBlock) + (ThunkBlockAllocationSize - m_currentARMThunkBlockRemaining);
    m_currentARMThunkBlockRemaining -= 4;

    return ptr;
}

#ifndef THUNK_MANAGER_H
#define THUNK_MANAGER_H

#include <shared_mutex>
#include <unordered_map>

class ThunkManager {
public:
    using X86ThunkTarget = void (*)(void);

    static void *allocateARMToX86ThunkCall(X86ThunkTarget x86SideTarget);
    static X86ThunkTarget lookupARMToX86ThunkCall(void *armCallAddress);

    static constexpr uint16_t ARMToX86ThunkCallSVC = 'T';

private:
    static void *allocateARMThunkAddressLocked();

    static constexpr size_t ThunkBlockAllocationSize = 65536;

    static std::shared_mutex m_thunkTableMutex;
    static std::unordered_map<X86ThunkTarget, void *> m_thunkX86ToArmTableForward;
    static std::unordered_map<void *, X86ThunkTarget> m_thunkX86ToArmTableReverse;
    static void *m_currentARMThunkBlock;
    static size_t m_currentARMThunkBlockRemaining;
};

#endif

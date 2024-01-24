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

    static X86ThunkTarget allocateX86ToARMThunkCall(void *key, X86ThunkTarget functionToCall);

private:
    class BumpAllocator {
    public:
        BumpAllocator();
        ~BumpAllocator();

        BumpAllocator(const BumpAllocator &other) = delete;
        BumpAllocator &operator =(const BumpAllocator &other) = delete;

        void *allocate(size_t size);

    private:
        static constexpr size_t ThunkBlockAllocationSize = 65536;
        unsigned char *m_currentBlock;
        unsigned char *m_currentBlockEnd;
    };


    static std::shared_mutex m_thunkTableMutex;
    static std::unordered_map<X86ThunkTarget, void *> m_thunkX86ToArmTableForward;
    static std::unordered_map<void *, X86ThunkTarget> m_thunkX86ToArmTableReverse;

    static std::unordered_map<void *, X86ThunkTarget> m_thunkArmToX86TableForward;

    static BumpAllocator m_armThunkAllocator;
    static BumpAllocator m_x86ThunkAllocator;
#if 0
    static void *m_currentARMThunkBlock;
    static size_t m_currentARMThunkBlockRemaining;
#endif
};

#endif

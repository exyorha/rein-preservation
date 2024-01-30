#ifndef THUNK_MANAGER_H
#define THUNK_MANAGER_H

#include <shared_mutex>
#include <unordered_map>

class ThunkManager {
public:
    static constexpr uint16_t ARMToX86ThunkCallSVC = 'T';

    using X86ThunkTarget = void (*)(void);

    ThunkManager();
    ~ThunkManager();

    ThunkManager(const ThunkManager &other) = delete;
    ThunkManager &operator =(const ThunkManager &other) = delete;

    void *allocateARMToX86ThunkCall(void *key, X86ThunkTarget functionToCallOnX86);
    void *lookupARMToX86ThunkCall(void *armCallAddress, ThunkManager::X86ThunkTarget *functionToCall = nullptr);

    X86ThunkTarget allocateX86ToARMThunkCall(void *key, X86ThunkTarget functionToCall);
    void *lookupX86ToARMThunkCall(X86ThunkTarget thunk);

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

    struct ARMThunk {
        uint32_t insn;
        X86ThunkTarget functionToCall;
    };

    std::shared_mutex m_thunkTableMutex;
    std::unordered_map<void *, void *> m_thunkX86ToArmTableForward;
    std::unordered_map<void *, void *> m_thunkX86ToArmTableReverse;

    std::unordered_map<void *, X86ThunkTarget> m_thunkArmToX86TableForward;
    std::unordered_map<X86ThunkTarget, void *> m_thunkArmToX86TableReverse;

    BumpAllocator m_armThunkAllocator;
    BumpAllocator m_x86ThunkAllocator;
};

#endif

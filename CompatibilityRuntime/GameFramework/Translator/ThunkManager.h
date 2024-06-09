#ifndef THUNK_MANAGER_H
#define THUNK_MANAGER_H

#include <shared_mutex>
#include <unordered_map>

#include <Translator/BumpAllocator.h>

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

    struct ARMThunk {
        static constexpr uint32_t Instruction =  UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMToX86ThunkCallSVC) << 5);

        uint32_t insn;
        X86ThunkTarget functionToCall;
        void *key;
    };

    std::shared_mutex m_thunkTableMutex;
    std::unordered_map<void *, void *> m_thunkX86ToArmTableForward;

    std::unordered_map<void *, X86ThunkTarget> m_thunkArmToX86TableForward;
    std::unordered_map<X86ThunkTarget, void *> m_thunkArmToX86TableReverse;

    BumpAllocator m_armThunkAllocator;
    BumpAllocator m_x86ThunkAllocator;
};

#endif

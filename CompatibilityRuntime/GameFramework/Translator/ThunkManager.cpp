#include <Translator/ThunkManager.h>
#include <Translator/thunking.h>

#include <mutex>
#include <cstring>

ThunkManager::ThunkManager() = default;

ThunkManager::~ThunkManager() = default;

void *ThunkManager::allocateARMToX86ThunkCall(void *key, X86ThunkTarget x86FunctionToCall) {
    if(!key || !x86FunctionToCall)
        throw std::logic_error("cannot thunk to null");

    {
        std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkX86ToArmTableForward.find(key);
        if(it != m_thunkX86ToArmTableForward.end()) {
            return it->second;
        }
    }

    {
        std::unique_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkX86ToArmTableForward.find(key);
        if(it != m_thunkX86ToArmTableForward.end()) {
            return it->second;
        }

        auto address = static_cast<ARMThunk *>(m_armThunkAllocator.allocate(sizeof(ARMThunk)));
        address->insn = ARMThunk::Instruction;
        address->functionToCall = x86FunctionToCall;
        address->key = key;

        m_thunkX86ToArmTableForward.emplace(key, address);

        return address;
    }
}

void *ThunkManager::lookupARMToX86ThunkCall(void *armCallAddress, ThunkManager::X86ThunkTarget *functionToCall) {
    auto thunk = static_cast<ARMThunk *>(armCallAddress);
    if(thunk->insn != ARMThunk::Instruction) {
        /*
         * Not our thunk.
         */
        return nullptr;
    }

    if(functionToCall) {
        *functionToCall = thunk->functionToCall;
    }

    return thunk->key;
}

ThunkManager::X86ThunkTarget ThunkManager::allocateX86ToARMThunkCall(void *key, ThunkManager::X86ThunkTarget functionToCall) {
    if(!functionToCall)
        throw std::logic_error("cannot thunk to null");


    {
        std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkArmToX86TableForward.find(key);
        if(it != m_thunkArmToX86TableForward.end()) {
            return it->second;
        }
    }

    {
        std::unique_lock<std::shared_mutex> locker(m_thunkTableMutex);

        auto it = m_thunkArmToX86TableForward.find(key);
        if(it != m_thunkArmToX86TableForward.end()) {
            return it->second;
        }

        static const uint8_t thunkCodeTemplate[]{
            // mov rax, [rip + key]
            0x48, 0x8B, 0x05, 0x11, 0x00, 0x00, 0x00,
            // NOTE: while a shorter instruction (with a 32-bit offst) can be
            // encoded here, it won't result in a shorter *thunk*, since it'll
            // require padding bytes
            // mov <fs:gs>:[<placeholder 64-bit offset>], rax
#if defined(_WIN32)
            0x65, // gs:
#else
            0x64, // fs:
#endif
            0x48, 0xA3, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
            // jmp [rip + target]
            0xFF, 0x25, 0x08, 0x00, 0x00, 0x00
            // Follows in the thunk structure:
            // key: 8-byte 'key' pointer
            // target: 8-byte 'target' pointer
        };

        static_assert((sizeof(thunkCodeTemplate) % sizeof(uintptr_t)) == 0, "the length of the thunk code must be a multiple of the pointer size - insert alignment padding");

        /*
         * 40 bytes.
         */
        struct Thunk {
            unsigned char code[sizeof(thunkCodeTemplate)];
            void *key;
            ThunkManager::X86ThunkTarget functionToCall;
        };

        auto thunk = static_cast<Thunk *>(m_x86ThunkAllocator.allocate(sizeof(Thunk)));
        memcpy(thunk->code, thunkCodeTemplate, sizeof(thunkCodeTemplate));

        *reinterpret_cast<int64_t __attribute__((aligned(1))) *>(&thunk->code[10]) = getOffsetOfThunkUtilitySlot();

        thunk->key = key;
        thunk->functionToCall = functionToCall;

        auto thunkFunc = reinterpret_cast<X86ThunkTarget>(thunk);

        m_thunkArmToX86TableForward.emplace(key, thunkFunc);
        m_thunkArmToX86TableReverse.emplace(thunkFunc, key);

        return thunkFunc;
    }

}

void *ThunkManager::lookupX86ToARMThunkCall(X86ThunkTarget thunk) {
    std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

    auto it = m_thunkArmToX86TableReverse.find(thunk);

    if(it != m_thunkArmToX86TableReverse.end())
        return it->second;

    return nullptr;
}

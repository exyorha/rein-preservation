#include <Translator/ThunkManager.h>
#include <Translator/thunking.h>

#include <sys/mman.h>

#include <system_error>
#include <mutex>
#include <cstring>

std::shared_mutex ThunkManager::m_thunkTableMutex;
std::unordered_map<void *, void *> ThunkManager::m_thunkX86ToArmTableForward;
std::unordered_map<void *, void *> ThunkManager::m_thunkX86ToArmTableReverse;
std::unordered_map<void *, ThunkManager::X86ThunkTarget> ThunkManager::m_thunkArmToX86TableForward;
std::unordered_map<ThunkManager::X86ThunkTarget, void *> ThunkManager::m_thunkArmToX86TableReverse;

ThunkManager::BumpAllocator ThunkManager::m_armThunkAllocator;
ThunkManager::BumpAllocator ThunkManager::m_x86ThunkAllocator;

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
        address->insn = UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMToX86ThunkCallSVC) << 5);
        address->functionToCall = x86FunctionToCall;

        m_thunkX86ToArmTableForward.emplace(key, address);
        m_thunkX86ToArmTableReverse.emplace(address, key);

        return address;
    }
}

void *ThunkManager::lookupARMToX86ThunkCall(void *armCallAddress, ThunkManager::X86ThunkTarget *functionToCall) {
    std::shared_lock<std::shared_mutex> locker(m_thunkTableMutex);

    auto it = m_thunkX86ToArmTableReverse.find(armCallAddress);

    if(it != m_thunkX86ToArmTableReverse.end()) {
        auto key = it->second;

        if(functionToCall) {
            auto thunk = static_cast<ARMThunk *>(armCallAddress);
            *functionToCall = thunk->functionToCall;
        }

        return key;
    }

    return nullptr;
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
            // mov fs:[<placeholder 64-bit offset>], rax
            // TODO: for Windows (would Windows be ever implemented, must be 'gs' here - 0x65 prefix
            0x64, 0x48, 0xA3, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
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

ThunkManager::BumpAllocator::BumpAllocator() : m_currentBlock(nullptr), m_currentBlockEnd(nullptr) {

}

ThunkManager::BumpAllocator::~BumpAllocator() = default;

void *ThunkManager::BumpAllocator::allocate(size_t size) {
    if(m_currentBlockEnd - m_currentBlock < size) {
        m_currentBlock = static_cast<unsigned char *>(
            mmap(nullptr, ThunkBlockAllocationSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
        );
        if(m_currentBlock == MAP_FAILED)
            throw std::system_error(errno, std::generic_category());

        m_currentBlockEnd = m_currentBlock + ThunkBlockAllocationSize;
    }

    auto ptr = m_currentBlock;
    m_currentBlock += size;

    return ptr;
}

#include <Translator/thunking.h>
#include <Translator/ThunkManager.h>

#include "GlobalContext.h"

#include <cinttypes>

#if defined(_WIN32)
#include <windows.h>
#include <winternl.h>
#include <Windows/WindowsError.h>

class WindowsTLSSlot {
public:
    WindowsTLSSlot();
    ~WindowsTLSSlot();

    WindowsTLSSlot(const WindowsTLSSlot &other) = delete;
    WindowsTLSSlot &operator =(const WindowsTLSSlot &other) = delete;

    inline void set(void *value) {
        TlsSetValue(m_slot, value);
    }

    inline void *get() {
        return TlsGetValue(m_slot);
    }

    inline DWORD slot() const {
        return m_slot;
    }

private:
    DWORD m_slot;
};

WindowsTLSSlot::WindowsTLSSlot() {

    m_slot = TlsAlloc();
    if(m_slot == TLS_OUT_OF_INDEXES)
        WindowsError::throwLastError();

    if(m_slot >= 64)
        throw std::runtime_error("too many TLS slots already allocated");
}

WindowsTLSSlot::~WindowsTLSSlot() {
    TlsFree(m_slot);
}

static WindowsTLSSlot thunkUtilitySlot;

#else

thread_local void *thunkUtilitySlot;
#endif

static const uint16_t ARMCallEndSVC = 0xE0;

static bool processJITExit(JITThreadContext &context, const SVCExit &exit) {
    if(exit.svc == ThunkManager::ARMToX86ThunkCallSVC) {
        ThunkManager::X86ThunkTarget invokable;

        auto key = GlobalContext::get().thunkManager().lookupARMToX86ThunkCall(reinterpret_cast<void *>(context.pc - 4), &invokable);
        if(!key)
            panic("ARMToX86ThunkCallSVC at an unknown thunk address");

        auto savedLR = context.lr();

        writeThunkUtilitySlot(key);
        invokable();

        context.pc = savedLR;

        return true;
    } else if(exit.svc == ARMCallEndSVC) {
        return false;
    } else {
        panic("unknown SVC 0x%04X in armcall\n", exit.svc);
    }
}

static bool processJITExit(JITThreadContext &context, const DiversionExit &diversion) {
    auto savedLR = context.lr();

    diversion.diversion->invoke();

    context.pc = savedLR;

    return true;
}

void runARMCall(JITThreadContext &context) {

    static const uint32_t stopSVC = UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMCallEndSVC) << 5);

    context.lr() = reinterpret_cast<uintptr_t>(&stopSVC);

    bool keepRunning;

    do {
        auto exitReason = GlobalContext::get().jit().runToSVC(context);
        keepRunning = std::visit([&context](const auto &reason) -> bool {
            return processJITExit(context, reason);
        }, exitReason);
    } while(keepRunning);
}


uintptr_t retrieveX86CallPointerSizedArgument(int position) {
    uintptr_t value;

    auto &context = JITThreadContext::get();

    if(position < 8) {
        value = context.gprs[position];
    } else {
        panic("stack argument passing is not implemented yet, cannot fetch an argument no. %d\n", position);
    }

    return value;
}

void storeX86CallPointerSizedResult(uintptr_t result) {
    auto &context = JITThreadContext::get();
    context.gprs[0] = result;
}

void storeX86CallStructureResult(const void *data, size_t size) {
    auto &context = JITThreadContext::get();

    memcpy(&context.gprs[0], data, size);
}

uintptr_t fetchARMCallPointerSizedResult() {

    auto &context = JITThreadContext::get();

    return  context.gprs[0];
}

void storeARMCallPointerSizedArgument(int position, uintptr_t value) {
    auto &context = JITThreadContext::get();

    if(position < 8) {
        context.gprs[position] = value;
    } else {
        panic("stack argument passing is not implemented yet, cannot store an argument no. %d\n", position);
    }
}

void storeX86CallFloatResult(float result) {
    auto &context = JITThreadContext::get();

    context.vectors[0][0] = std::bit_cast<uint32_t>(result);
    context.vectors[0][1] = 0;
}

void storeX86CallFloatResult(double result) {
    auto &context = JITThreadContext::get();

    context.vectors[0][0] = std::bit_cast<uint64_t>(result);
    context.vectors[0][1] = 0;
}

void storeX86CallFloatResult(long double result) {
    auto &context = JITThreadContext::get();

    auto val = std::bit_cast<__int128_t>(result);

    context.vectors[0][0] = static_cast<uint64_t>(val);
    context.vectors[0][1] = static_cast<uint64_t>(val >> 64);
}


void fetchX86CallFloatingPointArgument(int position, float &out) {
    if(position < 8) {
        auto &context = JITThreadContext::get();
        out = std::bit_cast<float>(static_cast<uint32_t>(context.vectors[position][0]));
    } else {
        panic("stack argument passing is not implemented yet, cannot fetch an argument no. %d\n", position);
    }
}

void fetchX86CallFloatingPointArgument(int position, double &out) {
    if(position < 8) {
        auto &context = JITThreadContext::get();
        out = std::bit_cast<double>(static_cast<uint64_t>(context.vectors[position][0]));
    } else {
        panic("stack argument passing is not implemented yet, cannot fetch an argument no. %d\n", position);
    }
}

void fetchX86CallFloatingPointArgument(int position, long double &out) {
    if(position < 8) {
        auto &context = JITThreadContext::get();

        __int128_t i128 = (static_cast<__int128_t>(context.vectors[position][1]) << 64) | context.vectors[position][0];

        out = std::bit_cast<long double>(i128);
    } else {
        panic("stack argument passing is not implemented yet, cannot fetch an argument no. %d\n", position);
    }
}

#if defined(_WIN32)
intptr_t getOffsetOfThunkUtilitySlot() {
    return offsetof(TEB, TlsSlots) + thunkUtilitySlot.slot() * sizeof(uintptr_t);
}

void *readThunkUtilitySlot() {
    return thunkUtilitySlot.get();
}

void writeThunkUtilitySlot(void *value) {
    thunkUtilitySlot.set(value);
}

#endif


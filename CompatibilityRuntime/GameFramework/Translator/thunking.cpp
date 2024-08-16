#include <Translator/thunking.h>
#include <Translator/ThunkManager.h>
#include <Translator/WrappedARMException.h>

#include <Bionic/BionicCallouts.h>

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
static const uint16_t ARMCallExceptionSVC = 0xE1;

static bool processJITExit(JITThreadContext &context, const SVCExit &exit) {
    if(exit.svc == ThunkManager::ARMToX86ThunkCallSVC) {
        ThunkManager::X86ThunkTarget invokable;

        auto key = GlobalContext::get().thunkManager().lookupARMToX86ThunkCall(reinterpret_cast<void *>(context.pc - 4), &invokable);
        if(!key)
            panic("ARMToX86ThunkCallSVC at an unknown thunk address");

        auto savedLR = context.thunkLR;
        context.thunkLR = context.lr();

        writeThunkUtilitySlot(key);

        invokable();

        context.pc = context.thunkLR;
        context.thunkLR = savedLR;

        return true;
    } else if(exit.svc == ARMCallExceptionSVC) {
        auto exceptionCode = context.gprs[0];
        auto exceptionObject = reinterpret_cast<void *>(context.gprs[1]);

        printf("ARM call exited due to ARM-side exception; exception code: 0x%016" PRIx64 ", exception object: %p\n",
            exceptionCode, exceptionObject);

        throw WrappedARMException(exceptionCode, exceptionObject);

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

    if(!arm_bionic_armcall_catcher)
        throw std::logic_error("cannot do arm calls until the armcall catcher is set");

    /*
     * The thunk starts with a 4-byte nop. Skip it. It's there to make the unwinder happier.
     */
    context.lr() = reinterpret_cast<uintptr_t>(arm_bionic_armcall_catcher) + 4;

    bool keepRunning;

    do {
        auto exitReason = GlobalContext::get().jit().runToSVC(context);
        keepRunning = std::visit([&context](const auto &reason) -> bool {
            return processJITExit(context, reason);
        }, exitReason);
    } while(keepRunning);
}

void rethrowWrappedARMExceptionFromX86Call(const WrappedARMException &exception) {
    /*
     * This can only be called from an arm->x86 thunk, with no other frames on
     * the ARM stack. We fudge the ARM machine context in such a way that the
     * ARM execution will resume instead from the landing pad in bionic, which
     * will then do _Unwind_RaiseException.
     */

    printf("rethrowing wrapped ARM exception, object %p\n", exception.exceptionObject());

    auto &context = JITThreadContext::get();

    context.lr() = context.thunkLR;
    context.thunkLR = reinterpret_cast<uintptr_t>(arm_bionic_armcall_rethrower);
    context.gprs[0] = reinterpret_cast<uintptr_t>(exception.exceptionObject());
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


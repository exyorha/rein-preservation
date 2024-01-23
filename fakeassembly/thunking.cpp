#include "thunking.h"

#include <cinttypes>

void runARMCall(JITThreadContext &context) {

    static const uint16_t ARMCallEndSVC = 0xE0;

    static const uint32_t stopSVC = UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMCallEndSVC) << 5);

    printf("Starting armcall on JIT thread %p, calling 0x%016" PRIx64 "\n", &context, context.pc);

    uint32_t exitSVC;

    context.gprs[30] = reinterpret_cast<uintptr_t>(&stopSVC);

    do {
        exitSVC = JIT::runToSVC(context);

        if(exitSVC == ThunkManager::ARMToX86ThunkCallSVC) {
            auto x86Side = ThunkManager::lookupARMToX86ThunkCall(reinterpret_cast<void *>(context.pc - 4));
            if(!x86Side)
                panic("ARMToX86ThunkCallSVC at an unknown thunk address");

            printf("  ARM->x86 call of %p\n", x86Side);

            x86Side();

            printf("  returning to LR 0x%016" PRIx64 "\n", context.gprs[30]);

            context.pc = context.gprs[30];
        } else if(exitSVC != ARMCallEndSVC) {
            panic("unknown SVC 0x%04X in armcall\n", exitSVC);
        }
    } while(exitSVC != ARMCallEndSVC);
}


uintptr_t retrieveX86CallPointerSizedArgument(int position) {
    uintptr_t value;

    auto &context = JITThreadContext::get();

    if(position < 8) {
        value = context.gprs[position];
    } else {
        panic("stack argument passing is not implemented yet, cannot fetch an argument no. %d\n", position);
    }

    printf("Fetched pointer-sized argument %d: 0x%016" PRIx64 "\n", position, value);

    return value;
}

void storeX86CallPointerSizedResult(uintptr_t result) {
    printf("Store pointer-sized result: 0x%016" PRIx64 "\n", result);

    auto &context = JITThreadContext::get();
    context.gprs[0] = result;
}

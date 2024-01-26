#include <Translator/thunking.h>
#include <Translator/ThunkManager.h>

#include <cinttypes>

thread_local void *thunkUtilitySlot;

void runARMCall(JITThreadContext &context) {

    static const uint16_t ARMCallEndSVC = 0xE0;

    static const uint32_t stopSVC = UINT32_C(0xD4000001) | (static_cast<uint32_t>(ARMCallEndSVC) << 5);

    uint32_t exitSVC;

    context.lr() = reinterpret_cast<uintptr_t>(&stopSVC);

    do {
        exitSVC = JIT::runToSVC(context);

        if(exitSVC == ThunkManager::ARMToX86ThunkCallSVC) {
            ThunkManager::X86ThunkTarget invokable;

            auto key = ThunkManager::lookupARMToX86ThunkCall(reinterpret_cast<void *>(context.pc - 4), &invokable);
            if(!key)
                panic("ARMToX86ThunkCallSVC at an unknown thunk address");

            auto savedLR = context.lr();

            thunkUtilitySlot = key;
            invokable();

            context.pc = savedLR;
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

    return value;
}

void storeX86CallPointerSizedResult(uintptr_t result) {
    auto &context = JITThreadContext::get();
    context.gprs[0] = result;
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

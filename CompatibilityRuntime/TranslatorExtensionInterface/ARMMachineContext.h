#ifndef ARM_MACHINE_CONTEXT_H
#define ARM_MACHINE_CONTEXT_H

#include <cstdint>
#include <array>

struct ARMMachineContext {
    using ARMVectorRegister = std::array<std::uint64_t, 2>;

    uint64_t pc;
    uint64_t sp;
    std::array<std::uint64_t, 31> gprs;
    std::array<ARMVectorRegister, 32> vectors;
    uint32_t fpcr;
    uint32_t fpsr;
    uint32_t pstate;
};

#endif

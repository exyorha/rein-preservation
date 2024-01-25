#include "JIT.h"
#include "dynarmic/interface/A64/config.h"
#include "dynarmic/interface/halt_reason.h"
#include "support.h"
#include "JITThreadContext.h"

#include <cstdio>
#include <cinttypes>
#include <signal.h>

std::mutex JIT::m_globalJITLock;
std::optional<JIT> JIT::m_jit;

JIT::JIT() :
    /*
     * We currently have a global JIT lock, so only one CPU can be waiting on a monitor.
     */
    m_exclusiveMonitor(1) {

    const char *debugging = getenv("ARM_DEBUG");

    Dynarmic::A64::UserConfig config;
    config.callbacks = this;
    config.silently_mirror_page_table = false;
    config.enable_cycle_counting = false;
    config.fastmem_address_space_bits = 64;
    config.silently_mirror_fastmem = false;
    config.use_null_fastmem = true;
    config.fastmem_exclusive_access = true;
    config.recompile_on_exclusive_fastmem_failure = false;
    config.recompile_on_fastmem_failure = false;
    config.global_monitor = &m_exclusiveMonitor;
    config.tpidr_el0 = &m_currentTPIDR_EL0;
    if(debugging)
        config.check_halt_on_memory_access = true;

    m_dynarmic.emplace(config);

    if(debugging) {
        m_gdbStub.emplace(debugging, *m_dynarmic);
    }
}

JIT::~JIT() = default;

uint32_t JIT::doRunToSVC(JITThreadContext &context) {
#if 0
    struct SignalGuard {
        SignalGuard() {
            sigset_t armSignals;
            sigemptyset(&armSignals);
            sigaddset(&armSignals, SIGPWR);
            sigaddset(&armSignals, SIGXCPU);
            sigprocmask(SIG_BLOCK, &armSignals, &oldmask);
        }

        ~SignalGuard() {
            sigprocmask(SIG_SETMASK, &oldmask, nullptr);
        }

    private:
        sigset_t oldmask;

    } signalGuard;
#endif
    context.apply(*m_dynarmic);
    m_currentTPIDR_EL0 = context.tpidr_el0;

    m_exitingOnSVC.reset();

    Dynarmic::HaltReason haltReason;
    do {

        if(m_gdbStub.has_value()) {
            m_gdbStub->beforeEnteringJIT();
        }

        if(m_gdbStub.has_value() && m_gdbStub->isSingleStepping()) {

            haltReason = m_dynarmic->Step();

        } else {

            haltReason = m_dynarmic->Run();
        }

        if(Dynarmic::Has(haltReason, Dynarmic::HaltReason::UserDefined2)) {

            /*
             * ARM exception.
             */
            auto exception = m_exitingOnException.value();
            m_exitingOnException.reset();

            if(m_gdbStub.has_value()) {
                printf("delivering the exception %u to the debugger, PC 0x%" PRIx64 "\n", exception, m_dynarmic->GetPC());

                m_dynarmic->SetPC(m_pcAtFault.value());
                m_pcAtFault.reset();

                unsigned int signal;

                if(exception == Dynarmic::A64::Exception::Breakpoint) {
                    signal = SIGTRAP;
                } else if(exception == Dynarmic::A64::Exception::NoExecuteFault) {
                    signal = SIGSEGV;
                } else {
                    signal = SIGILL;
                }

                m_gdbStub->stopped(signal);

            } else {
                printf("The ARM exception %u has occurred.\n", exception);
                dumpMachineContext();
                panic("ARM exception\n");
            }
        } else if(Dynarmic::Has(haltReason, Dynarmic::HaltReason::MemoryAbort)) {

            /*
             * Unmapped memory access.
             */

            if(m_gdbStub.has_value()) {
                printf("delivering the memory fault to the debugger, PC 0x%" PRIx64 "\n", m_dynarmic->GetPC());

                m_gdbStub->stopped(SIGSEGV);
            } else {
                dumpMachineContext();
                panic("unmapped memory access\n");
            }
        } else if(Dynarmic::Has(haltReason, Dynarmic::HaltReason::Step)) {
            if(m_gdbStub.has_value())
                m_gdbStub->stepped();
        } else if(haltReason != Dynarmic::HaltReason::UserDefined1) {
            panic("JIT has exited on a condition other than SVC call: 0x%04X; ARM PC: 0x%016" PRIx64 "\n", haltReason, context.pc);
        }

    } while(!Dynarmic::Has(haltReason, Dynarmic::HaltReason::UserDefined1));

    context.tpidr_el0 = m_currentTPIDR_EL0;
    context.capture(*m_dynarmic);



    if(!m_exitingOnSVC.has_value())
        throw std::logic_error("exit SVC not set");

    auto svc = *m_exitingOnSVC;
    m_exitingOnSVC.reset();

    return svc;

}

void JIT::stopDebuggerIfAttached(unsigned int signal) {
    std::unique_lock<std::mutex> locker(m_globalJITLock);

    return jitInstanceLocked()->doStopDebuggerIfAttached(signal);
}

void JIT::doStopDebuggerIfAttached(unsigned int signal) {
    if(m_gdbStub.has_value())
        m_gdbStub->stopped(signal);
}

uint32_t JIT::runToSVC(JITThreadContext &thread) {
    std::unique_lock<std::mutex> locker(m_globalJITLock);

    return jitInstanceLocked()->doRunToSVC(thread);
}

void JIT::flushInstructionCacheLockedInternal(uintptr_t addr, size_t size) {
    return jitInstanceLocked()->m_dynarmic->InvalidateCacheRange(addr, size);
}

JIT *JIT::jitInstanceLocked() {
    if(!m_jit.has_value())
        m_jit.emplace();

    return &*m_jit;
}

std::optional<std::uint32_t> JIT::MemoryReadCode(Dynarmic::A64::VAddr vaddr) {
    return *reinterpret_cast<const uint32_t *>(vaddr);
}

std::uint8_t JIT::MemoryRead8(Dynarmic::A64::VAddr vaddr) {
    printf("unmapped 8-bit memory read from %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

    return {};
}

std::uint16_t JIT::MemoryRead16(Dynarmic::A64::VAddr vaddr) {
    printf("unmapped 16-bit memory read from %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

    return {};
}

std::uint32_t JIT::MemoryRead32(Dynarmic::A64::VAddr vaddr) {
    printf("unmapped 32-bit memory read from %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

    return {};
}

void JIT::dumpMachineContext() {
    fputs("ARM state:\n", stdout);
    for(unsigned int gpr = 0; gpr < 31; gpr++) {
        printf("R%-2u:  0x%016" PRIx64, gpr, m_dynarmic->GetRegister(gpr));
        if((gpr % 2) == 0)
            fputs("    ", stdout);
        else
            fputs("\n", stdout);
    }

    printf("\nPC:   0x%016" PRIx64 "    SP:    0x%016" PRIx64 "\n", m_dynarmic->GetPC(), m_dynarmic->GetSP());

    printf("PSTATE: 0x%08" PRIx32 "   FPCR: 0x%08" PRIx32 "   FPSR: 0x%08" PRIx32 "\n",
           m_dynarmic->GetPstate(), m_dynarmic->GetFpcr(), m_dynarmic->GetFpsr());
#if 0 // this is *x86* disassembly, not very useful
    printf("Instruction disassembly:\n");

    auto insns = m_dynarmic->Disassemble();
    for(const auto &insn: insns) {
        printf("  %s\n", insn.c_str());
    }
#endif
}

std::uint64_t JIT::MemoryRead64(Dynarmic::A64::VAddr vaddr) {

    printf("unmapped 64-bit memory read from %016" PRIx64 "; ARM PC %016" PRIx64 "\n", vaddr,
          m_dynarmic->GetPC());

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

    return {};

}

Dynarmic::A64::Vector JIT::MemoryRead128(Dynarmic::A64::VAddr vaddr) {
    printf("unmapped 128-bit memory read from %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

    return {};
}

void JIT::MemoryWrite8(Dynarmic::A64::VAddr vaddr, std::uint8_t value) {
    printf("unmapped 8-bit memory write to %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

}

void JIT::MemoryWrite16(Dynarmic::A64::VAddr vaddr, std::uint16_t value) {
    printf("unmapped 16-bit memory write to %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

}

void JIT::MemoryWrite32(Dynarmic::A64::VAddr vaddr, std::uint32_t value) {
    printf("unmapped 32-bit memory write to %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

}

void JIT::MemoryWrite64(Dynarmic::A64::VAddr vaddr, std::uint64_t value) {
    printf("unmapped 64-bit memory write to %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);

}

void JIT::MemoryWrite128(Dynarmic::A64::VAddr vaddr, Dynarmic::A64::Vector value) {
    printf("unmapped 128-bit memory write to %016" PRIx64 "\n", vaddr);

    m_dynarmic->HaltExecution(Dynarmic::HaltReason::MemoryAbort);
}

void JIT::InterpreterFallback(Dynarmic::A64::VAddr pc, size_t num_instructions) {
    printf("interpreter fallback is required to execute from %016" PRIx64 "\n", pc);
}

void JIT::CallSVC(std::uint32_t swi) {
    m_exitingOnSVC.emplace(swi);
    m_dynarmic->HaltExecution();
}

void JIT::ExceptionRaised(Dynarmic::A64::VAddr pc, Dynarmic::A64::Exception exception) {

    printf("ARM exception %d raised at PC 0x%" PRIx64 "\n", exception, pc);

    m_exitingOnException.emplace(exception);
    m_pcAtFault.emplace(pc);
    m_dynarmic->HaltExecution(Dynarmic::HaltReason::UserDefined2);
}

void JIT::AddTicks(std::uint64_t ticks) {

}

std::uint64_t JIT::GetTicksRemaining() {
    return 65536;
}

std::uint64_t JIT::GetCNTPCT() {
    return 0;
}

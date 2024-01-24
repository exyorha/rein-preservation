#include "JIT.h"
#include "dynarmic/interface/halt_reason.h"
#include "support.h"
#include "JITThreadContext.h"

#include <cstdio>
#include <cinttypes>

std::recursive_mutex JIT::m_globalJITLock;
std::optional<JIT> JIT::m_jit;

JIT::JIT() :
    /*
     * We currently have a global JIT lock, so only one CPU can be waiting on a monitor.
     */
    m_exclusiveMonitor(1) {
    Dynarmic::A64::UserConfig config;
    config.callbacks = this;
    config.silently_mirror_page_table = false;
    config.enable_cycle_counting = false;
    config.fastmem_address_space_bits = 64;
    config.silently_mirror_fastmem = false;
    config.use_null_fastmem = true;
    config.fastmem_exclusive_access = true;
    config.global_monitor = &m_exclusiveMonitor;

    m_dynarmic.emplace(config);
}

JIT::~JIT() = default;

uint32_t JIT::doRunToSVC(JITThreadContext &context) {
    printf("JIT: starting JIT execution; PC: 0x%016" PRIx64 "\n", context.pc);

    context.apply(*m_dynarmic);

    m_exitingOnSVC.reset();

    auto result = m_dynarmic->Run();

    context.capture(*m_dynarmic);

    if(result != Dynarmic::HaltReason::UserDefined1) {
        panic("JIT has exited on a condition other than SVC call: 0x%04X; ARM PC: 0x%016" PRIx64 "\n", result, context.pc);
    }

    if(!m_exitingOnSVC.has_value())
        throw std::logic_error("exit SVC not set");

    auto svc = *m_exitingOnSVC;
    m_exitingOnSVC.reset();

    printf("JIT: JIT has exited on SVC 0x%04X; PC: 0x%016" PRIx64 "\n", svc, context.pc);

    return svc;

}

uint32_t JIT::runToSVC(JITThreadContext &thread) {
    std::unique_lock<std::recursive_mutex> locker(m_globalJITLock);

    return jitInstanceLocked()->doRunToSVC(thread);
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
    panic("unmapped 8-bit memory read from %016" PRIx64 "\n", vaddr);
}

std::uint16_t JIT::MemoryRead16(Dynarmic::A64::VAddr vaddr) {
    panic("unmapped 16-bit memory read from %016" PRIx64 "\n", vaddr);
}

std::uint32_t JIT::MemoryRead32(Dynarmic::A64::VAddr vaddr) {
    panic("unmapped 32-bit memory read from %016" PRIx64 "\n", vaddr);
}

std::uint64_t JIT::MemoryRead64(Dynarmic::A64::VAddr vaddr) {
    panic("unmapped 64-bit memory read from %016" PRIx64 "\n", vaddr);
}

Dynarmic::A64::Vector JIT::MemoryRead128(Dynarmic::A64::VAddr vaddr) {
    panic("unmapped 128-bit memory read from %016" PRIx64 "\n", vaddr);
}

void JIT::MemoryWrite8(Dynarmic::A64::VAddr vaddr, std::uint8_t value) {
    panic("unmapped 8-bit memory write to %016" PRIx64 "\n", vaddr);
}

void JIT::MemoryWrite16(Dynarmic::A64::VAddr vaddr, std::uint16_t value) {
    panic("unmapped 16-bit memory write to %016" PRIx64 "\n", vaddr);
}

void JIT::MemoryWrite32(Dynarmic::A64::VAddr vaddr, std::uint32_t value) {
    panic("unmapped 32-bit memory write to %016" PRIx64 "\n", vaddr);
}

void JIT::MemoryWrite64(Dynarmic::A64::VAddr vaddr, std::uint64_t value) {
    panic("unmapped 64-bit memory write to %016" PRIx64 "\n", vaddr);
}

void JIT::MemoryWrite128(Dynarmic::A64::VAddr vaddr, Dynarmic::A64::Vector value) {
    panic("unmapped 128-bit memory write to %016" PRIx64 "\n", vaddr);
}

void JIT::InterpreterFallback(Dynarmic::A64::VAddr pc, size_t num_instructions) {
    panic("interpreter fallback is required to execute from %016" PRIx64 "\n", pc);
}

void JIT::CallSVC(std::uint32_t swi) {
    m_exitingOnSVC.emplace(swi);
    m_dynarmic->HaltExecution();
}

void JIT::ExceptionRaised(Dynarmic::A64::VAddr pc, Dynarmic::A64::Exception exception) {

    panic("ARM exception %d raised at PC 0x" PRIx64 "\n", exception, pc);
}

void JIT::AddTicks(std::uint64_t ticks) {

}

std::uint64_t JIT::GetTicksRemaining() {
    return 65536;
}

std::uint64_t JIT::GetCNTPCT() {
    return 0;
}

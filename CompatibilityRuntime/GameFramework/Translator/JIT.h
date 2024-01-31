#ifndef JIT_H
#define JIT_H

#include <mutex>
#include <optional>
#include <variant>

#include <dynarmic/interface/A64/config.h>
#include <dynarmic/interface/A64/a64.h>
#include <dynarmic/interface/exclusive_monitor.h>

#include <GDB/GDBStub.h>

class JITThreadContext;

struct SVCExit {
    uint32_t svc;
};

struct Diversion;

struct DiversionExit {
    const Diversion *diversion;
};

using JITExitReason = std::variant<SVCExit, DiversionExit>;

class JIT final : private Dynarmic::A64::UserCallbacks {
public:
    JIT();
    ~JIT();

    JIT(const JIT &other) = delete;
    JIT &operator =(const JIT &other) = delete;

    JITExitReason runToSVC (JITThreadContext &thread);
    void stopDebuggerIfAttached(unsigned int signal);
    void flushInstructionCache(uintptr_t addr, size_t size);
    void flushInstructionCacheLockedInternal(uintptr_t addr, size_t size);

private:
    std::optional<std::uint32_t> MemoryReadCode(Dynarmic::A64::VAddr vaddr) override;

    std::uint8_t MemoryRead8(Dynarmic::A64::VAddr vaddr) override;
    std::uint16_t MemoryRead16(Dynarmic::A64::VAddr vaddr) override;
    std::uint32_t MemoryRead32(Dynarmic::A64::VAddr vaddr) override;
    std::uint64_t MemoryRead64(Dynarmic::A64::VAddr vaddr) override;
    Dynarmic::A64::Vector MemoryRead128(Dynarmic::A64::VAddr vaddr) override;

    void MemoryWrite8(Dynarmic::A64::VAddr vaddr, std::uint8_t value) override;
    void MemoryWrite16(Dynarmic::A64::VAddr vaddr, std::uint16_t value) override;
    void MemoryWrite32(Dynarmic::A64::VAddr vaddr, std::uint32_t value) override;
    void MemoryWrite64(Dynarmic::A64::VAddr vaddr, std::uint64_t value) override;
    void MemoryWrite128(Dynarmic::A64::VAddr vaddr, Dynarmic::A64::Vector value) override;

    void InterpreterFallback(Dynarmic::A64::VAddr pc, size_t num_instructions) override;

    void CallSVC(std::uint32_t swi) override;
    void ExceptionRaised(Dynarmic::A64::VAddr pc, Dynarmic::A64::Exception exception) override;

    void AddTicks(std::uint64_t ticks) override;
    std::uint64_t GetTicksRemaining() override;
    std::uint64_t GetCNTPCT() override;

    void doStopDebuggerIfAttached(unsigned int signal);

    void dumpMachineContext();

private:
    std::mutex m_globalJITLock;
    std::optional<Dynarmic::A64::Jit> m_dynarmic;
    std::optional<uint32_t> m_exitingOnSVC;
    std::optional<uint64_t> m_pcAtFault;
    std::optional<Dynarmic::A64::Exception> m_exitingOnException;
    Dynarmic::ExclusiveMonitor m_exclusiveMonitor;
    std::optional<GDBStub> m_gdbStub;
    uint64_t m_currentTPIDR_EL0;
};

#endif

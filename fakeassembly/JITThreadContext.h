#ifndef JIT_THREAD_CONTEXT_H
#define JIT_THREAD_CONTEXT_H

#include <memory>
#include <cstdint>
#include <array>

#include <dynarmic/interface/A64/a64.h>

class JITThreadContext {
public:

    JITThreadContext();
    ~JITThreadContext();

    JITThreadContext(const JITThreadContext &other) = delete;
    JITThreadContext &operator =(const JITThreadContext &other) = delete;

    static JITThreadContext &get();

    void apply(Dynarmic::A64::Jit &jit) const;
    void capture(const Dynarmic::A64::Jit &jit);

    uint64_t pc;
    uint64_t sp;
    std::array<std::uint64_t, 31> gprs;
    std::array<Dynarmic::A64::Vector, 32> vectors;
    uint32_t fpcr;
    uint32_t fpsr;

private:
    static constexpr size_t ThreadStackSize = 128 * 1024;

    static thread_local std::unique_ptr<JITThreadContext> m_jitThread;
    void *m_threadStack;
};

#endif



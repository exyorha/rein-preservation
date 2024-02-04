#ifndef JIT_THREAD_CONTEXT_H
#define JIT_THREAD_CONTEXT_H

#include <memory>
#include <cstdint>
#include <array>
#include <mutex>

#include <dynarmic/interface/A64/a64.h>

class GarbageCollectorThreadVisitor {
protected:
    GarbageCollectorThreadVisitor() = default;
    ~GarbageCollectorThreadVisitor() = default;

    GarbageCollectorThreadVisitor(const GarbageCollectorThreadVisitor &other) = default;
    GarbageCollectorThreadVisitor &operator =(const GarbageCollectorThreadVisitor &other) = default;

    GarbageCollectorThreadVisitor(GarbageCollectorThreadVisitor &&other) noexcept = default;
    GarbageCollectorThreadVisitor &operator =(GarbageCollectorThreadVisitor &&other) noexcept = default;

public:
    virtual void visit(void *stackBottom, void *stackTop, std::array<std::uint64_t, 31> &gprs) = 0;
};

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
    uint32_t pstate;

    inline uint64_t &fp() {
        return gprs[29];
    }

    inline uint64_t fp() const {
        return gprs[29];
    }

    inline uint64_t &lr() {
        return gprs[30];
    }

    inline uint64_t lr() const {
        return gprs[30];
    }

    /*
     * These two routines work with *16-byte* stack slots!
     */
    void push(uint64_t value);
    uint64_t pop();

    inline void *threadARMStack() {
        return m_threadStack;
    }

    inline size_t threadARMStackSize() {
        return ThreadStackSize;
    }

    uint64_t tpidr_el0;

    std::array<uint64_t, 32> fakeTLS;

    bool stoppedWorld;

    static inline void collectThreadStacks(GarbageCollectorThreadVisitor *visitor) {
        ThreadContextRegistration::collectThreadStacks(visitor);
    }

    void collectThreadStack(GarbageCollectorThreadVisitor *visitor);

private:
    static constexpr size_t ThreadStackSize = 512 * 1024;

    class ThreadContextRegistration {
    public:
        explicit ThreadContextRegistration(JITThreadContext *registeredContext);
        ~ThreadContextRegistration();

        ThreadContextRegistration(const ThreadContextRegistration &other) = delete;
        ThreadContextRegistration &operator =(const ThreadContextRegistration &other) = delete;

        static void collectThreadStacks(GarbageCollectorThreadVisitor *visitor);

    private:
        JITThreadContext *m_registeredContext;

        static std::mutex m_contextListMutex;
        static std::vector<JITThreadContext *> m_contextList;
    };

    static thread_local std::unique_ptr<JITThreadContext> m_jitThread;
    void *m_threadStack;
    std::optional<ThreadContextRegistration> m_registration;
};

#endif



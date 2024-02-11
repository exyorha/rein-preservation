#ifndef JIT_THREAD_CONTEXT_H
#define JIT_THREAD_CONTEXT_H

#include <memory>
#include <cstdint>
#include <array>
#include <mutex>
#include <list>

#include <dynarmic/interface/A64/a64.h>

#include <Bionic/BionicThreading.h>

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

class JITThreadContextPtr;
class JITThreadLocalContextPtr;
class JoinableThreadManager;

class JITThreadContext {
private:
     ~JITThreadContext();

public:
    JITThreadContext();
    JITThreadContext(void *providedStack, size_t providedStackSize);

    void addReference() noexcept;
    void release() noexcept;

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

    inline void *threadARMStack() const {
        return m_threadStack;
    }

    inline size_t threadARMStackSize() const {
        return m_threadStackSize;
    }

    uint64_t tpidr_el0;

    std::array<uintptr_t, BIONIC_TLS_SLOTS> bionicTLS;
    std::array<uintptr_t, BIONIC_PTHREAD_KEY_COUNT> pthreadKeys;

    bool stoppedWorld;

    static inline void collectThreadStacks(GarbageCollectorThreadVisitor *visitor) {
        ThreadContextRegistration::collectThreadStacks(visitor);
    }

    void collectThreadStack(GarbageCollectorThreadVisitor *visitor);

    void startNewHostThread(void *(*func)(void *arg), void *arg);
    void detach();
    void join(void **result);
    [[noreturn]] void exitHostThread(void *result);

    void threadStateTeardown() noexcept;

    static void clearCurrentThreadContext() noexcept;

    static inline void waitForAllThreadsToExit() noexcept {
        ThreadContextRegistration::waitForAllThreadsToExit();
    }
private:
    class ThreadContextRegistration {
    public:
        explicit ThreadContextRegistration(JITThreadContext *registeredContext);
        ~ThreadContextRegistration();

        ThreadContextRegistration(const ThreadContextRegistration &other) = delete;
        ThreadContextRegistration &operator =(const ThreadContextRegistration &other) = delete;

        static void collectThreadStacks(GarbageCollectorThreadVisitor *visitor);

        static void waitForAllThreadsToExit() noexcept;

    private:
        JITThreadContext *m_registeredContext;

        static std::mutex m_contextListMutex;
        static std::condition_variable m_exitCondvar;
        static std::vector<JITThreadContext *> m_contextList;
    };

    static void hostThreadEntry(JITThreadContextPtr &&context, void *(*func)(void *arg), void *arg);

    static thread_local JITThreadLocalContextPtr m_jitThread;
    static JoinableThreadManager m_joinableManager;

    std::atomic_uintptr_t m_referenceCount;
    void *m_threadStack;
    size_t m_threadStackSize;
    bool m_threadStackAllocated;
    void *m_threadResult;
    std::optional<ThreadContextRegistration> m_registration;
    std::thread m_hostThread;
};

class JITThreadContextPtr {
public:
    explicit JITThreadContextPtr(JITThreadContext *pointer = nullptr) noexcept;
    ~JITThreadContextPtr();

    JITThreadContextPtr(const JITThreadContextPtr &other) noexcept;
    JITThreadContextPtr &operator =(const JITThreadContextPtr &other) noexcept;

    inline JITThreadContext *get() const noexcept {
        return m_pointer;
    }

    inline JITThreadContext &operator *() const {
        return *m_pointer;
    }

    inline JITThreadContext *operator ->() const {
        return m_pointer;
    }

private:
    JITThreadContext *m_pointer;
};

class JITThreadLocalContextPtr {
public:
    explicit JITThreadLocalContextPtr() noexcept;
    ~JITThreadLocalContextPtr();

    JITThreadLocalContextPtr(const JITThreadLocalContextPtr &other) = delete;
    JITThreadLocalContextPtr &operator =(const JITThreadLocalContextPtr &other) noexcept = delete;

    JITThreadLocalContextPtr(const JITThreadContextPtr &other) noexcept;
    JITThreadLocalContextPtr &operator =(const JITThreadContextPtr &other) noexcept;

    inline JITThreadContext *get() const noexcept {
        return m_pointer;
    }

    inline JITThreadContext &operator *() const {
        return *m_pointer;
    }

    inline JITThreadContext *operator ->() const {
        return m_pointer;
    }

private:
    JITThreadContext *m_pointer;
};

class JoinableThreadManager {
public:
    JoinableThreadManager();
    ~JoinableThreadManager();

    JoinableThreadManager(const JoinableThreadManager &other) = delete;
    JoinableThreadManager &operator =(const JoinableThreadManager &other) = delete;

    void addJoinableThread(JITThreadContext *ctx);
    void removeJoinableThread(JITThreadContext *ctx);

private:
    std::mutex m_mutex;
    std::list<JITThreadContextPtr> m_unjoinedThreads;
};

#endif



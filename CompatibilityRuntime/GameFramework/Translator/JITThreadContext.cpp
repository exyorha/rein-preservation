#include <Translator/JITThreadContext.h>
#include <Translator/ThreadContextAssociation.h>

#include <Bionic/BionicCallouts.h>

#ifdef _WIN32
#include <Windows/WindowsError.h>
#include <windows.h>
#else
#include <sys/mman.h>
#endif


#include <cstring>
#include <system_error>
#include <algorithm>
#include <thread>

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
#include "gc.h"
#include <Translator/GCHooks.h>
#endif

JoinableThreadManager JITThreadContext::m_joinableManager;

std::mutex JITThreadContext::ThreadContextRegistration::m_contextListMutex;
std::condition_variable JITThreadContext::ThreadContextRegistration::m_exitCondvar;
std::vector<JITThreadContext *> JITThreadContext::ThreadContextRegistration::m_contextList;

JITThreadContext::JITThreadContext(void *providedStack, size_t providedStackSize) :
    stoppedWorld(false), m_referenceCount(1),
    m_threadStack(providedStack), m_threadStackSize(providedStackSize), m_threadStackAllocated(false), m_threadResult(nullptr) {

    sp = 0;
    pc = 0;
    fpcr = 0;
    fpsr = 0;
    pstate = UINT32_C(0x80000000);

    memset(&gprs, 0xBA, sizeof(gprs));
    memset(&vectors, 0xBA, sizeof(vectors));

    if(m_threadStack == nullptr || m_threadStackSize == 0) {
        m_threadStackAllocated = true;
        m_threadStackSize = 128 * 1024;

#ifdef _WIN32
        m_threadStack = VirtualAlloc(nullptr, m_threadStackSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if(m_threadStack == nullptr)
            WindowsError::throwLastError();
#else

        m_threadStack = mmap(nullptr, m_threadStackSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK | MAP_GROWSDOWN, -1, 0);
        if(m_threadStack == MAP_FAILED)
            throw std::system_error(errno, std::generic_category());
#endif
    }

    sp = reinterpret_cast<uintptr_t>(m_threadStack) + m_threadStackSize;

    tpidr_el0 = reinterpret_cast<uint64_t>(&bionicTLS);
    memset(&bionicTLS, 0, sizeof(bionicTLS));

    bionicTLS[BIONIC_TLS_SLOT_SELF] = reinterpret_cast<uintptr_t>(&bionicTLS);
    bionicTLS[BIONIC_TLS_SLOT_THREAD_ID] = reinterpret_cast<uintptr_t>(this); // this is what pthread_self is
    bionicTLS[BIONIC_TLS_SLOT_KEY_DATA] = reinterpret_cast<uintptr_t>(&pthreadKeys);

    memset(&pthreadKeys, 0, sizeof(pthreadKeys));

    m_registration.emplace(this);
}

JITThreadContext::JITThreadContext() : JITThreadContext(nullptr, 0) {

}

JITThreadContext::~JITThreadContext() {
    m_registration.reset();

    if(m_threadStackAllocated) {
#ifdef _WIN32
        VirtualFree(m_threadStack, 0, MEM_RELEASE);
#else
        munmap(m_threadStack, m_threadStackSize);
#endif
    }
}

void JITThreadContext::addReference() noexcept {
    m_referenceCount.fetch_add(1);
}

void JITThreadContext::release() noexcept {
    if(m_referenceCount.fetch_sub(1) == 1) {
        delete this;
    }
}

JITThreadContext &JITThreadContext::get() {
    auto thisContext = getJITContextOfCurrentThread();
    if(!thisContext) {
        thisContext = new JITThreadContext;
        setJITContextOfCurrentThread(thisContext);
        thisContext->release();
    }

    return *thisContext;
}

void JITThreadContext::apply(Dynarmic::A64::Jit &jit) const {
    jit.SetSP(sp);
    jit.SetPC(pc);
    jit.SetRegisters(gprs);
    jit.SetVectors(vectors);
    jit.SetFpcr(fpcr);
    jit.SetFpsr(fpsr);
    jit.SetPstate(pstate);
}

void JITThreadContext::capture(const Dynarmic::A64::Jit &jit) {
    sp = jit.GetSP();
    pc = jit.GetPC();
    gprs = jit.GetRegisters();
    vectors = jit.GetVectors();
    fpcr = jit.GetFpcr();
    fpsr = jit.GetFpsr();
    pstate = jit.GetPstate();
}

void JITThreadContext::collectThreadStack(GarbageCollectorThreadVisitor *visitor) {
    auto stackBottom = reinterpret_cast<void *>(sp);
    auto stackTop = reinterpret_cast<char *>(m_threadStack) + m_threadStackSize;

    visitor->visit(stackBottom, stackTop, gprs);
}

void JITThreadContext::exitHostThread(void *result) {
    m_threadResult = result;

#if defined(_WIN32)
    ExitThread(0);
#else
    pthread_exit(nullptr);
#endif
}

JITThreadContext::ThreadContextRegistration::ThreadContextRegistration(JITThreadContext *registeredContext) : m_registeredContext(registeredContext) {
    std::unique_lock<std::mutex> locker(m_contextListMutex);
    m_contextList.emplace_back(m_registeredContext);
}

JITThreadContext::ThreadContextRegistration::~ThreadContextRegistration() {
    {
        std::unique_lock<std::mutex> locker(m_contextListMutex);
        m_contextList.erase(std::remove(m_contextList.begin(), m_contextList.end(), m_registeredContext), m_contextList.end());
    }
    m_exitCondvar.notify_all();
}

void JITThreadContext::ThreadContextRegistration::waitForAllThreadsToExit() noexcept {
    std::unique_lock<std::mutex> locker(m_contextListMutex);

    auto result = m_exitCondvar.wait_for(locker, std::chrono::milliseconds(250), []() -> bool {
        return m_contextList.size() == 0;
    });

    if(!result) {
        fprintf(stderr, "waitForAllThreadsToExit(): %zu threads still remain, but the timeout has been reached, continuing with the shutdown. Translation services will not be available to the remaining threads from this point on.\n", m_contextList.size());
    }
}

void JITThreadContext::ThreadContextRegistration::collectThreadStacks(GarbageCollectorThreadVisitor *visitor) {
    std::unique_lock<std::mutex> locker(m_contextListMutex);

    for(auto context: m_contextList) {
        context->collectThreadStack(visitor);
    }
}

void JITThreadContext::startNewHostThread(void *(*func)(void *arg), void *arg) {
    if(m_hostThread.joinable()) {
        throw std::runtime_error("JITThreadContext: already started this thread");
    }

    JITThreadContextPtr pointer(this);
    addReference();

    m_joinableManager.addJoinableThread(this);

    m_hostThread = std::thread(&JITThreadContext::hostThreadEntry, std::move(pointer), func, arg);
}

void JITThreadContext::hostThreadEntry(JITThreadContextPtr &&context, void *(*func)(void *arg), void *arg) {
    if(getJITContextOfCurrentThread())
        throw std::runtime_error("JITThreadContext::hostThreadEntry: newly created host thread already has a JIT context set up\n");

    setJITContextOfCurrentThread(context.get());

    getJITContextOfCurrentThread()->m_threadResult = func(arg);
}

void JITThreadContext::join(void **result) {
    m_hostThread.join();

    *result = m_threadResult;

    m_joinableManager.removeJoinableThread(this);
}

void JITThreadContext::detach() {
    m_hostThread.detach();

    m_joinableManager.removeJoinableThread(this);
}

void JITThreadContext::threadStateInitialization() noexcept {
#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
    GC_stack_base sb;

    sb.mem_base = getPlatformSpecificStackBottomForThisThread();

    auto result = GC_register_my_thread(&sb);
    if(result != GC_SUCCESS) {
        fprintf(stderr, "GC_register_my_thread has failed for %p: %d\n", this, result);
    }
#endif
}

void JITThreadContext::threadStateTeardown() noexcept {

    bionic_teardown_thread();

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
    GC_unregister_my_thread();
#endif
}

void JITThreadContext::clearCurrentThreadContext() noexcept {
    setJITContextOfCurrentThread(nullptr);
}

JITThreadContextPtr::JITThreadContextPtr(JITThreadContext *ptr) noexcept : m_pointer(ptr) {

}

JITThreadContextPtr::~JITThreadContextPtr() {
    if(m_pointer) {
        m_pointer->release();
    }
}

JITThreadContextPtr::JITThreadContextPtr(const JITThreadContextPtr &other) noexcept : m_pointer(other.m_pointer) {
    if(m_pointer)
        m_pointer->addReference();
}

JITThreadContextPtr &JITThreadContextPtr::operator =(const JITThreadContextPtr &other) noexcept {
    auto old = m_pointer;

    m_pointer = other.m_pointer;
    if(m_pointer)
        m_pointer->addReference();

    if(old) {
        old->release();
    }

    return *this;
}

JoinableThreadManager::JoinableThreadManager() = default;

JoinableThreadManager::~JoinableThreadManager() = default;

void JoinableThreadManager::addJoinableThread(JITThreadContext *ctx) {
    JITThreadContextPtr newThread(ctx);
    ctx->addReference();

    {
        std::unique_lock<std::mutex> locker(m_mutex);

        for(const auto &thread: m_unjoinedThreads) {
            if(thread.get() == newThread.get())
                throw std::logic_error("JoinableThreadManager::addJoinableThread: the thread is already on the list");
        }

        m_unjoinedThreads.emplace_back(std::move(newThread));
    }
}

void JoinableThreadManager::removeJoinableThread(JITThreadContext *ctx) {
    /*
     * We create a new reference to thread we're removing, because we may
     * remove the last reference to the thread, and we don't want the
     * thread to be destroyed with the mutex still held.
     */
    JITThreadContextPtr extraRef(ctx);
    extraRef->addReference();


    {
        std::unique_lock<std::mutex> locker(m_mutex);

        auto it = std::find_if(m_unjoinedThreads.begin(), m_unjoinedThreads.end(), [ctx](const auto &ptr) -> bool { return ptr.get() == ctx; });
        if(it == m_unjoinedThreads.end()) {
            throw std::logic_error("JoinableThreadManager::addJoinableThread: the thread is not on the list");
        }

        m_unjoinedThreads.erase(it);
    }
}

void JoinableThreadManager::forgetAll() {
    std::unique_lock<std::mutex> locker(m_mutex);

    if(!m_unjoinedThreads.empty()) {
        fprintf(stderr, "JoinableThreadManager: %zu unjoined threads remain, forgetting about them\n",
                m_unjoinedThreads.size());

        m_unjoinedThreads.clear();
    }
}

void JITThreadContext::waitForAllThreadsToExit() {
    m_joinableManager.forgetAll();
    ThreadContextRegistration::waitForAllThreadsToExit();
}

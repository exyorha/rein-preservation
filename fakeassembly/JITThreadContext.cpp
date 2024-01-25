#include "JITThreadContext.h"

#include <dynarmic/interface/A64/a64.h>

#include <sys/mman.h>

#include <cstring>
#include <system_error>

thread_local std::unique_ptr<JITThreadContext> JITThreadContext::m_jitThread;

JITThreadContext::JITThreadContext() : sp(0), pc(0), fpcr(0), fpsr(0), pstate(UINT32_C(0x80000000)) {
    memset(&gprs, 0xBA, sizeof(gprs));
    memset(&vectors, 0xBA, sizeof(vectors));

    m_threadStack = mmap(nullptr, ThreadStackSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK | MAP_GROWSDOWN, -1, 0);
    if(m_threadStack == MAP_FAILED)
        throw std::system_error(errno, std::generic_category());

    sp = reinterpret_cast<uintptr_t>(m_threadStack) + ThreadStackSize;

    tpidr_el0 = reinterpret_cast<uint64_t>(&fakeTLS);
    memset(&fakeTLS, 0, sizeof(fakeTLS));
}

JITThreadContext::~JITThreadContext() {
    munmap(m_threadStack, ThreadStackSize);
}

JITThreadContext &JITThreadContext::get() {
    auto &thisContext = m_jitThread;

    if(!thisContext) {
        printf("creating a new JIT context in slot %p\n", &thisContext);
        thisContext = std::make_unique<JITThreadContext>();
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

void JITThreadContext::push(uint64_t value) {
    sp -= 16;

    *reinterpret_cast<uint64_t *>(sp) = value;
}

uint64_t JITThreadContext::pop() {
    auto value = *reinterpret_cast<uint64_t *>(sp);

    sp += 16;

    return value;
}


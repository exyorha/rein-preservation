#include <Translator/GCHooks.h>
#include <Translator/JITThreadContext.h>

#include <ELF/Image.h>

#include <cstdarg>

#include "GlobalContext.h"
#include "support.h"
#include "thunking.h"

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
#include "gc.h"
#include "private/gc_priv.h"
#include "gc_mark.h"
#endif

static size_t *totalStackSize;
static void (*arm_GC_push_all_stack)(void *bottom, void *top);

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
static void GC_CALLBACK warnProc(char *format, GC_word value);
static void GC_CALLBACK abortProc(const char *msg);

int GC_dont_gc = 0;
int GC_print_stats = 0;
int GC_is_initialized = 1;
char *GC_stackbottom = nullptr;
GC_warn_proc GC_current_warn_proc = warnProc;
GC_abort_func GC_on_abort = abortProc;
word GC_total_stacksize;
GC_sp_corrector_proc GC_sp_corrector = nullptr;
GC_on_thread_event_proc GC_on_thread_event;
GC_word GC_gc_no;

#ifdef _WIN32
CRITICAL_SECTION GC_write_cs;
size_t GC_page_size;
#endif

static void GC_CALLBACK warnProc(char *format, GC_word value) {
    fprintf(stderr, format, value);
}

static void GC_CALLBACK abortProc(const char *msg) {
    if(msg)
        fputs(msg, stderr);
}

static void *gcAllocReplacement(size_t lb) {
    auto ptr = malloc(lb);
    if(ptr && lb) {
        memset(ptr, 0, lb);
    }

    return ptr;
}

void * GC_generic_malloc_inner(size_t lb, int k) {
    (void)k;
    return gcAllocReplacement(lb);
}

void * GC_malloc_uncollectable(size_t size) {
    return gcAllocReplacement(size);
}

void GC_free_inner(void * p) {
    free(p);
}

void GC_free(void *p) {
    GC_free_inner(p);
}

void GC_CALL GC_noop1(word) {

}

void GC_start_mark_threads(void) {}

void GC_init(void) {}

void * GC_CALL GC_call_with_stack_base(GC_stack_base_func /* fn */,
                                        void * /* arg */) {
    abort();
}

GC_INNER ptr_t GC_approx_sp(void) {
    return static_cast<ptr_t>(__builtin_frame_address(0));
}

void GC_end_stubborn_change(const void *) {}

void GC_log_printf(const char * format, ...) {
    va_list args;

    va_start(args, format);

    vprintf(format, args);

    va_end(args);
}

void initializeHostGC() {
    printf("Host-side GC initialization\n");

#ifdef _WIN32
    GC_page_size = GlobalContext::PageSize;
    InitializeCriticalSection(&GC_write_cs);
    InitializeCriticalSection(&GC_allocate_ml);
#endif

    GC_stackbottom = static_cast<char *>(getPlatformSpecificStackBottomForThisThread());

    GC_thr_init();
}

void *getPlatformSpecificStackBottomForThisThread() {

#ifdef _WIN32
    auto teb = reinterpret_cast<PNT_TIB>(NtCurrentTeb());

    return teb->StackBase;
#else
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    void *stackaddr;
    size_t stacksize;
    pthread_attr_getstack(&attr, &stackaddr, &stacksize);

    return static_cast<unsigned char *>(stackaddr) + stacksize;
#endif
}
#endif

class BoehmGCThreadVisitor final : public GarbageCollectorThreadVisitor {
public:
    BoehmGCThreadVisitor();
    ~BoehmGCThreadVisitor();

    BoehmGCThreadVisitor(const BoehmGCThreadVisitor &other) = delete;
    BoehmGCThreadVisitor &operator =(const BoehmGCThreadVisitor &other) = delete;

    inline size_t totalStackSize() const {
        return m_totalStackSize;
    }

    void visit(void *stackBottom, void *stackTop, std::array<std::uint64_t, 31> &gprs) override;

private:
    size_t m_totalStackSize;
};

BoehmGCThreadVisitor::BoehmGCThreadVisitor() : m_totalStackSize(0) {

}

BoehmGCThreadVisitor::~BoehmGCThreadVisitor() = default;

static void markOnARMSide(void *stackBottom, void *stackTop) {
    //printf("Marking stack: %p - %p\n", stackBottom, stackTop);
    if(stackBottom != stackTop) {
        armcall(arm_GC_push_all_stack, stackBottom, stackTop);
    }
}

void BoehmGCThreadVisitor::visit(void *stackBottom, void *stackTop, std::array<std::uint64_t, 31> &gprs) {
    //printf("ARM thread: ARM stack %p-%p, gprs at %p\n", stackBottom, stackTop, gprs.data());
    markOnARMSide(stackBottom, stackTop);

    markOnARMSide(static_cast<void *>(gprs.data()), static_cast<void *>(gprs.data() + gprs.size()));

    m_totalStackSize += reinterpret_cast<uintptr_t>(stackTop) - reinterpret_cast<uintptr_t>(stackBottom);
}

/*
 * All of these functions have the ARM side signature of void(void), so no
 * argument or return translation is needed for them.
 */
static void GC_stop_world_diversion(const Diversion *diversion) {
    (void)diversion;

    //printf("GC stop world\n");

    GlobalContext::get().jit().stopWorld(JITThreadContext::get());
}

static void GC_start_world_diversion(const Diversion *diversion) {
    (void)diversion;

    //printf("GC start world\n");

    GlobalContext::get().jit().startWorld(JITThreadContext::get());
}

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
extern "C" {
    void GC_push_all_stacks(void);
}

void GC_push_all_stack_sections(ptr_t lo, ptr_t hi,
                        struct GC_traced_stack_sect_s *traced_stack_sect) {

    markOnARMSide(lo, hi);
}

void GC_push_all(void * bottom, void *top) {
    markOnARMSide(bottom, top);
}

void GC_push_all_stack(ptr_t bottom, ptr_t top) {
    markOnARMSide(bottom, top);
}

void GC_push_many_regs(const word *regs, unsigned count) {
    markOnARMSide(const_cast<word *>(regs), const_cast<word *>(regs + count));
}
#endif

static void GC_push_all_stacks_diversion(const Diversion *diversion) {
    (void)diversion;

    //printf("GC push all stacks\n");

    BoehmGCThreadVisitor visitor;

    /*
     * Collect the ARM stacks.
     */
    JITThreadContext::collectThreadStacks(&visitor);

#ifdef CR_GARBAGE_COLLECT_HOST_STACKS
    /*
     * Also collect the x86 stacks.
     */
    GC_stop_world();

    GC_push_all_stacks();

    GC_start_world();
#endif

    *totalStackSize = visitor.totalStackSize();
}

static void GC_stop_init_diversion(const Diversion *diversion) {
    (void)diversion;
}

void installGCHooks(const Image &il2cppImage) {
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25aad50), GC_stop_world_diversion, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25aae3c), GC_start_world_diversion, nullptr);
    arm_GC_push_all_stack = il2cppImage.displace<void (void *bottom, void *top)>(0x25afb48);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25b25d0), GC_push_all_stacks_diversion, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25b3020), GC_stop_init_diversion, nullptr);

    totalStackSize = il2cppImage.displace<size_t>(0x79a6720);
}


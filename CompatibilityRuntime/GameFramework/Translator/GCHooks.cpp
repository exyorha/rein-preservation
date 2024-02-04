#include <Translator/GCHooks.h>
#include <Translator/JITThreadContext.h>

#include "GlobalContext.h"
#include "support.h"
#include "thunking.h"

static size_t *totalStackSize;
static void (*arm_GC_push_all_stack)(void *bottom, void *top);

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

void BoehmGCThreadVisitor::visit(void *stackBottom, void *stackTop, std::array<std::uint64_t, 31> &gprs) {
    if(stackBottom != stackTop) {
        armcall(arm_GC_push_all_stack, stackBottom, stackTop);
    }

    armcall(arm_GC_push_all_stack, static_cast<void *>(gprs.data()), static_cast<void *>(gprs.data() + gprs.size()));

    m_totalStackSize = reinterpret_cast<uintptr_t>(stackTop) - reinterpret_cast<uintptr_t>(stackBottom);
}

/*
 * All of these functions have the ARM side signature of void(void), so no
 * argument or return translation is needed for them.
 */
static void GC_stop_world_diversion(const Diversion *diversion) {
    (void)diversion;

    GlobalContext::get().jit().stopWorld(JITThreadContext::get());
}

static void GC_start_world_diversion(const Diversion *diversion) {
    (void)diversion;

    GlobalContext::get().jit().startWorld(JITThreadContext::get());
}

static void GC_push_all_stacks(const Diversion *diversion) {
    (void)diversion;

    BoehmGCThreadVisitor visitor;

    JITThreadContext::collectThreadStacks(&visitor);

    *totalStackSize = visitor.totalStackSize();
}

static void GC_stop_init_diversion(const Diversion *diversion) {
    (void)diversion;
}

void installGCHooks(const Image &il2cppImage) {
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25902a4), GC_stop_world_diversion, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x2590390), GC_start_world_diversion, nullptr);
    arm_GC_push_all_stack = il2cppImage.displace<void (void *bottom, void *top)>(0x259509c);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x2597b24), GC_push_all_stacks, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x2598574), GC_stop_init_diversion, nullptr);

    totalStackSize = il2cppImage.displace<size_t>(0x7966aa0);

}

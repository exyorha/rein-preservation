#include <Translator/GCHooks.h>
#include <Translator/JITThreadContext.h>

#include "GlobalContext.h"
#include "support.h"

/*
 * All of these functions have the ARM side signature of void(void), so no
 * argument or return translation is needed for them.
 */
static void GC_stop_world_diversion(const Diversion *diversion) {
    (void)diversion;

    printf("GC_stop_world hook\n");
    GlobalContext::get().jit().stopWorld(JITThreadContext::get());
}

static void GC_start_world_diversion(const Diversion *diversion) {
    (void)diversion;

    printf("GC_start_world hook\n");
    GlobalContext::get().jit().startWorld(JITThreadContext::get());
}

static void GC_stop_init_diversion(const Diversion *diversion) {
    (void)diversion;

    printf("GC_stop_init hook\n");
}

void installGCHooks(const Image &il2cppImage) {
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x25902a4), GC_stop_world_diversion, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x2590390), GC_start_world_diversion, nullptr);
    GlobalContext::get().diversionManager().divert(il2cppImage.displace(0x2598574), GC_stop_init_diversion, nullptr);

}

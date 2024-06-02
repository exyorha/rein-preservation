#ifndef BIONIC_BIONIC_CALLOUTS_H
#define BIONIC_BIONIC_CALLOUTS_H

#include "Bionic/BionicThreading.h"
#include <Bionic/BionicABITypes.h>
#include <Translator/thunking.h>

class ElfSymbolSource;

void bindBionicCallouts(const ElfSymbolSource &bionic);

// Called before the bionic constructors to do the initialization normally done by the dynamic linker
using BionicInitLibcType = void (*)(BionicKernelArgumentBlock* args);

extern BionicInitLibcType arm_bionic_init_libc;

static inline void bionic_init_libc(BionicKernelArgumentBlock* args) {
    armcall(arm_bionic_init_libc, args);
}

// Called to set the bionic errno.
static inline void bionic_set_errno(int error) {
    JITThreadContext::get().bionicTLS[BIONIC_TLS_SLOT_ERRNO] = error;
}

// Called before the JIT thread context is torn down to tear down the Bionic
// thread context: pthread keys, pthread cleanups, and so on.
using BionicTeardownThread = void (*)(void);

extern BionicTeardownThread arm_bionic_teardown_thread;

static inline void bionic_teardown_thread(void) {
    armcall(arm_bionic_teardown_thread);
}

extern void *arm_bionic_armcall_catcher;
extern void *arm_bionic_armcall_rethrower;

#endif


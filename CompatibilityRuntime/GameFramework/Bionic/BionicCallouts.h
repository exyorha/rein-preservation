#ifndef BIONIC_BIONIC_CALLOUTS_H
#define BIONIC_BIONIC_CALLOUTS_H

#include <Bionic/BionicABITypes.h>
#include <Translator/thunking.h>

class Image;

void bindBionicCallouts(const Image &bionic);

// Called before the bionic constructors to do the initialization normally done by the dynamic linker
using BionicInitLibcType = void (*)(BionicKernelArgumentBlock* args);

extern BionicInitLibcType arm_bionic_init_libc;

static inline void bionic_init_libc(BionicKernelArgumentBlock* args) {
    armcall(arm_bionic_init_libc, args);
}

// Called to set the bionic errno.
// TODO: we can do it faster by mucking with TLS.
using BionicSetErrnoType = void (*)(int errorNumber);

extern BionicSetErrnoType arm_bionic_set_errno;

static inline void bionic_set_errno(int error) {
    armcall(arm_bionic_set_errno, error);
}

// Called before the JIT thread context is torn down to tear down the Bionic
// thread context: pthread keys, pthread cleanups, and so on.
using BionicTeardownThread = void (*)(void);

extern BionicTeardownThread arm_bionic_teardown_thread;

static inline void bionic_teardown_thread(void) {
    armcall(arm_bionic_teardown_thread);
}

#endif


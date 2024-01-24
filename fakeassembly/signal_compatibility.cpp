#include "signal_compatibility.h"
#include "support.h"
#include "thunking.h"

#include <signal.h>
#include <string.h>

struct sigaction_android {
    int sa_flags;
    union {
        sighandler_t mem_sa_handler;
        void (*mem_sa_sigaction)(int, siginfo_t*, void*);
    };
    sigset_t sa_mask;
    void (*sa_restorer)(void);
};

static bool isSignalPermittedToHaveAnARMHandler(int sig) {
    /*
     * Used by Boehm GC.
     */
    return sig == SIGPWR || sig == SIGXCPU;
}

template<typename T>
static inline T * thunkARMSignalHandler(T *func) {
    return reinterpret_cast<T *>(
        ThunkManager::allocateX86ToARMThunkCall(
            reinterpret_cast<void *>(func),
            reinterpret_cast<ThunkManager::X86ThunkTarget>(static_cast<T *>(x86CallThunk))));
}

int sigaction_compat(int sig, const struct sigaction_android *act, struct sigaction_android *oact) {
    struct sigaction native_act;
    memset(&native_act, 0, sizeof(native_act));

    struct sigaction native_oact;
    memset(&native_oact, 0, sizeof(native_oact));

    const struct sigaction *actPointer = nullptr;
    struct sigaction *oactPointer = nullptr;
    if(act) {
        if(act->sa_flags & 0x04000000) {
            panic("SA_RESTORER is set in a sigaction structure, this is not supported");
        }

        native_act.sa_flags = act->sa_flags;

        if(act->sa_flags & SA_SIGINFO) {
            if(!isSignalPermittedToHaveAnARMHandler(sig))
                panic("attempted to set an ARM handler %p (siginfo) for the signal %d\n",
                    act->mem_sa_sigaction, sig);

            native_act.sa_sigaction = thunkARMSignalHandler(act->mem_sa_sigaction);

        } else {
            if(act->mem_sa_handler == SIG_DFL || act->mem_sa_handler == SIG_IGN) {
                native_act.sa_handler = act->mem_sa_handler;
            } else if(!isSignalPermittedToHaveAnARMHandler(sig)) {
                panic("attempted to set an ARM handler %p (traditional) for the signal %d\n",
                      act->mem_sa_handler, sig);
            } else {
                native_act.sa_handler = thunkARMSignalHandler(act->mem_sa_handler);
            }
        }

        actPointer = &native_act;
    }

    if(oact) {
        oactPointer = &native_oact;
    }

    auto result = sigaction(sig, actPointer, oactPointer);

    if(result == 0) {
        if(oact) {
            panic("sigaction_compat: implement reading: flags %d\n", native_oact.sa_flags);
        }
    }

    return result;
}

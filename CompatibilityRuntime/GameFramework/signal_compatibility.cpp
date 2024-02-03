#include "signal_compatibility.h"
#include "support.h"
#include <Translator/thunking.h>

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

int sigaction_compat(int sig, const struct sigaction_android *act, struct sigaction_android *oact) {
    panic("sigaction_compat: installing signal handlers from the ARM code is not supported. GC hooks should be used to prevent Boehm GC from doing that.\n");

    errno = ENOSYS;
    return -1;
}

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

pid_t ___getpid(void);

void _exit(int status) {
    _Exit(status);
}

void __compatibility_runtime_set_errno(int error) {
    errno = error;
}

int __rt_sigprocmask(int what, const void* newset, void* oldset, size_t setSize) {
    (void)what;
    (void)newset;

    /*
     * Since we don't actually do signals, sigprocmask can be a no-op.
     */

    if(oldset)
        memset(oldset, 0, setSize);

    return 0;
}

pid_t getpid(void) {
    /*
     * We never fork, so our pid cache can never get stale.
     */

    static pid_t cached = 0;

    if(cached == 0) {
        cached = ___getpid();
    }

    return cached;
}

int gettimeofday(struct timeval *restrict tp, struct timezone *restrict tzp) {
    (void)tzp;

    struct timespec ts;

    if(clock_gettime(CLOCK_REALTIME, &ts) < 0)
        return -1;

    tp->tv_sec = ts.tv_sec;
    tp->tv_usec = (ts.tv_nsec + 500) / 1000;

    return 0;
}

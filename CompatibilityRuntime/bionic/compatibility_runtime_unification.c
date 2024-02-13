#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/utsname.h>

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

/*
 * Doesn't matter, because we don't do processes.
 */
pid_t getpid(void) {
    return 1;
}

uid_t getuid(void) {
    return 1;
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

int uname(struct utsname *name) {
    strncpy(name->sysname, "Linux", sizeof(name->sysname));
    strncpy(name->nodename, "localhost", sizeof(name->nodename));
    strncpy(name->release, "6.7", sizeof(name->release));
    strncpy(name->version, "CompatibilityRuntime", sizeof(name->version));
    strncpy(name->machine, "arm64", sizeof(name->machine));
    strncpy(name->domainname,"localhost.localdomain", sizeof(name->domainname));

    return 0;
}

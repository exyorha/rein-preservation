#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "private/libc_logging.h"

void _exit(int status) {
    _Exit(status);
}

int ___open(const char *pathname, int flags, mode_t mode);

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

int __open_2(const char* pathname, int flags) {
  if (__predict_false((flags & O_CREAT) != 0)) {
    __fortify_chk_fail("open(O_CREAT): called without specifying a mode", 0);
  }

  return ___open(pathname, flags, 0);
}
int open(const char* pathname, int flags, ...) {
  mode_t mode = 0;

  if ((flags & O_CREAT) != 0) {
    va_list args;
    va_start(args, flags);
    mode = (mode_t)va_arg(args, int);
    va_end(args);
  }

  return ___open(pathname, flags, mode);
}
__strong_alias(open64, open);

int creat(const char* pathname, mode_t mode) {
  return ___open(pathname, O_CREAT | O_TRUNC | O_WRONLY, mode);
}
__strong_alias(creat64, creat);

int fstatat(int fd, const char* path, struct stat* statbuf, int flags) {
  if(fd != AT_FDCWD) {
    errno = ENOSYS;
    return -1;
  }

  if(flags & AT_SYMLINK_NOFOLLOW) {
    return lstat(path, statbuf);
  } else {
    return stat(path, statbuf);
  }
}
__strong_alias(fstatat64, fstatat)

#include "Bionic/BionicABITypes.h"
#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <ctime>
#include <cerrno>

static int translateErrno(int hostPlatformError) {
    return hostPlatformError;
}

static bool translateClockID(clockid_t &host, bionic_clockid_t bionic) {
#ifdef _WIN32
    switch(bionic) {
        case BIONIC_CLOCK_MONOTONIC:
            host = CLOCK_MONOTONIC;
            break;

        case BIONIC_CLOCK_REALTIME:
            host = CLOCK_REALTIME;
            break;

        case BIONIC_CLOCK_PROCESS_CPUTIME_ID:
            host = CLOCK_PROCESS_CPUTIME_ID;
            break;

        case BIONIC_CLOCK_THREAD_CPUTIME_ID:
            host = CLOCK_THREAD_CPUTIME_ID;
            break;

        case BIONIC_CLOCK_REALTIME_COARSE:
            host = CLOCK_REALTIME_COARSE;
            break;

        default:
            return false;
    }

    return true;
#else
    host = bionic;
    return true;
#endif
}

static void translateTimespec(struct bionic_timespec *out, const struct timespec *in) {
    out->tv_sec = in->tv_sec;
    out->tv_nsec = in->tv_nsec;
}

static void translateTimespecToNative(struct timespec *out, const struct bionic_timespec *in) {
    out->tv_sec = in->tv_sec;
    out->tv_nsec = in->tv_nsec;
}

#ifndef _WIN32

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/utsname.h>

static void translateStat(const struct stat *native, struct bionic_stat *android) {
    android->st_dev = native->st_dev;
    android->st_ino = native->st_ino;
    android->st_mode = native->st_mode;
    android->st_nlink = native->st_nlink;
    android->st_uid = native->st_uid;
    android->st_gid = native->st_gid;
    android->st_rdev = native->st_rdev;
    android->st_size = native->st_size;
    android->st_blksize = native->st_blksize;
    android->st_blocks = native->st_blocks;
    translateTimespec(&android->st_atim, &native->st_atim);
    translateTimespec(&android->st_mtim, &native->st_mtim);
    translateTimespec(&android->st_ctim, &native->st_ctim);
}

int plat_openat(int fd, const char* path, int oflag, int mode) {
    auto result = openat(fd, path, oflag, mode);
    if(result < 0) {
        bionic_set_errno(translateErrno(errno));
    }

    return result;
}

int plat_fstat(int fd, bionic_stat *statbuf) {
    struct stat native;

    auto result = ::fstat(fd, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int plat_fstatat(int fd, const char *path, struct bionic_stat *statbuf, int flag) {
    struct stat native;

    auto result = ::fstatat(fd, path, &native, flag);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int plat_dup(int fd) {
    auto result = dup(fd);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_close(int fd) {
    auto result = close(fd);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

bionic_ssize_t plat_read(int fd, void *dest, size_t size) {
    auto result = read(fd, dest, size);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

ssize_t plat_write(int fd, const void *dest, size_t size) {
    auto result = write(fd, dest, size);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

ssize_t plat_readlinkat(int dirfd, const char *pathname,
                        char *buf, size_t bufsiz) {

    auto result = readlinkat(dirfd, pathname, buf, bufsiz);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_faccessat(int fd, const char *path, int amode, int flag) {
    auto result = faccessat(fd, path, amode, flag);

    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
    auto result = renameat(olddirfd, oldpath, newdirfd, newpath);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_unlinkat(int dirfd, const char *pathname, int flags) {
    auto result = unlinkat(dirfd, pathname, flags);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_mkdirat(int dirfd, const char *pathname, bionic_mode_t mode) {
    auto result = mkdirat(dirfd, pathname, mode);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

bionic_off_t plat_lseek(int fildes, bionic_off_t offset, int whence) {
    auto result = lseek(fildes, offset, whence);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;

}
#endif

int plat_clock_gettime(bionic_clockid_t clockid, bionic_timespec *tp) {
    struct timespec tn;

    clockid_t host;
    if(!translateClockID(host, clockid)) {
        bionic_set_errno(EINVAL);
        return -1;
    }

    auto result = clock_gettime(host, &tn);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    translateTimespec(tp, &tn);

    return result;
}

int plat_clock_getres(bionic_clockid_t clockid, bionic_timespec *tp) {
    struct timespec tn;

    clockid_t host;
    if(!translateClockID(host, clockid)) {
        bionic_set_errno(EINVAL);
        return -1;
    }

    auto result = clock_getres(host, &tn);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    translateTimespec(tp, &tn);

    return result;
}

int plat_nanosleep(const bionic_timespec *rqtp, bionic_timespec *rmtp) {
    struct timespec native_rqtp;
    struct timespec native_rmtp;

    translateTimespecToNative(&native_rqtp, rqtp);

    auto result = nanosleep(&native_rqtp, &native_rmtp);

    if(rmtp) {
        translateTimespec(rmtp, &native_rmtp);
    }

    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

#ifndef _WIN32
void *plat_mmap(void *addr, size_t len, int prot, int flags, int fildes, bionic_off_t off) {
    auto result = mmap(addr, len, prot, flags, fildes, off);
    if(result == MAP_FAILED)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_mprotect(void *addr, size_t len, int prot) {
    auto result = mprotect(addr, len, prot);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_munmap(void *addr, size_t len) {
    auto result = munmap(addr, len);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

int plat_uname(bionic_utsname *name) {
    utsname host;
    auto result = uname(&host);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));
    else {
        strncpy(name->sysname, host.sysname, sizeof(name->sysname));
        strncpy(name->nodename, host.nodename, sizeof(name->nodename));
        strncpy(name->release, host.release, sizeof(name->release));
        strncpy(name->version, host.version, sizeof(name->version));
        strncpy(name->machine, host.machine, sizeof(name->machine));
        strncpy(name->release, host.release, sizeof(name->release));
    }

    return result;
}

#endif

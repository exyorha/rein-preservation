#include "Bionic/BionicABITypes.h"
#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <ctime>
#include <cerrno>

int translateErrno(int hostPlatformError) {
#ifdef _WIN32
    struct ErrorMapEntry {
        int host;
        int bionic;
    };

    static const ErrorMapEntry errorMap[] {
        { .host = 0, .bionic = 0 },
#ifdef EPERM
{ .host = EPERM, .bionic = BIONIC_EPERM },
#endif
#ifdef ENOENT
{ .host = ENOENT, .bionic = BIONIC_ENOENT },
#endif
#ifdef ESRCH
{ .host = ESRCH, .bionic = BIONIC_ESRCH },
#endif
#ifdef EINTR
{ .host = EINTR, .bionic = BIONIC_EINTR },
#endif
#ifdef EIO
{ .host = EIO, .bionic = BIONIC_EIO },
#endif
#ifdef ENXIO
{ .host = ENXIO, .bionic = BIONIC_ENXIO },
#endif
#ifdef E2BIG
{ .host = E2BIG, .bionic = BIONIC_E2BIG },
#endif
#ifdef ENOEXEC
{ .host = ENOEXEC, .bionic = BIONIC_ENOEXEC },
#endif
#ifdef EBADF
{ .host = EBADF, .bionic = BIONIC_EBADF },
#endif
#ifdef ECHILD
{ .host = ECHILD, .bionic = BIONIC_ECHILD },
#endif
#ifdef EAGAIN
{ .host = EAGAIN, .bionic = BIONIC_EAGAIN },
#endif
#ifdef ENOMEM
{ .host = ENOMEM, .bionic = BIONIC_ENOMEM },
#endif
#ifdef EACCES
{ .host = EACCES, .bionic = BIONIC_EACCES },
#endif
#ifdef EFAULT
{ .host = EFAULT, .bionic = BIONIC_EFAULT },
#endif
#ifdef ENOTBLK
{ .host = ENOTBLK, .bionic = BIONIC_ENOTBLK },
#endif
#ifdef EBUSY
{ .host = EBUSY, .bionic = BIONIC_EBUSY },
#endif
#ifdef EEXIST
{ .host = EEXIST, .bionic = BIONIC_EEXIST },
#endif
#ifdef EXDEV
{ .host = EXDEV, .bionic = BIONIC_EXDEV },
#endif
#ifdef ENODEV
{ .host = ENODEV, .bionic = BIONIC_ENODEV },
#endif
#ifdef ENOTDIR
{ .host = ENOTDIR, .bionic = BIONIC_ENOTDIR },
#endif
#ifdef EISDIR
{ .host = EISDIR, .bionic = BIONIC_EISDIR },
#endif
#ifdef EINVAL
{ .host = EINVAL, .bionic = BIONIC_EINVAL },
#endif
#ifdef ENFILE
{ .host = ENFILE, .bionic = BIONIC_ENFILE },
#endif
#ifdef EMFILE
{ .host = EMFILE, .bionic = BIONIC_EMFILE },
#endif
#ifdef ENOTTY
{ .host = ENOTTY, .bionic = BIONIC_ENOTTY },
#endif
#ifdef ETXTBSY
{ .host = ETXTBSY, .bionic = BIONIC_ETXTBSY },
#endif
#ifdef EFBIG
{ .host = EFBIG, .bionic = BIONIC_EFBIG },
#endif
#ifdef ENOSPC
{ .host = ENOSPC, .bionic = BIONIC_ENOSPC },
#endif
#ifdef ESPIPE
{ .host = ESPIPE, .bionic = BIONIC_ESPIPE },
#endif
#ifdef EROFS
{ .host = EROFS, .bionic = BIONIC_EROFS },
#endif
#ifdef EMLINK
{ .host = EMLINK, .bionic = BIONIC_EMLINK },
#endif
#ifdef EPIPE
{ .host = EPIPE, .bionic = BIONIC_EPIPE },
#endif
#ifdef EDOM
{ .host = EDOM, .bionic = BIONIC_EDOM },
#endif
#ifdef ERANGE
{ .host = ERANGE, .bionic = BIONIC_ERANGE },
#endif
#ifdef EDEADLK
{ .host = EDEADLK, .bionic = BIONIC_EDEADLK },
#endif
#ifdef ENAMETOOLONG
{ .host = ENAMETOOLONG, .bionic = BIONIC_ENAMETOOLONG },
#endif
#ifdef ENOLCK
{ .host = ENOLCK, .bionic = BIONIC_ENOLCK },
#endif
#ifdef ENOSYS
{ .host = ENOSYS, .bionic = BIONIC_ENOSYS },
#endif
#ifdef ENOTEMPTY
{ .host = ENOTEMPTY, .bionic = BIONIC_ENOTEMPTY },
#endif
#ifdef ELOOP
{ .host = ELOOP, .bionic = BIONIC_ELOOP },
#endif
#ifdef ENOMSG
{ .host = ENOMSG, .bionic = BIONIC_ENOMSG },
#endif
#ifdef EIDRM
{ .host = EIDRM, .bionic = BIONIC_EIDRM },
#endif
#ifdef ECHRNG
{ .host = ECHRNG, .bionic = BIONIC_ECHRNG },
#endif
#ifdef EL2NSYNC
{ .host = EL2NSYNC, .bionic = BIONIC_EL2NSYNC },
#endif
#ifdef EL3HLT
{ .host = EL3HLT, .bionic = BIONIC_EL3HLT },
#endif
#ifdef EL3RST
{ .host = EL3RST, .bionic = BIONIC_EL3RST },
#endif
#ifdef ELNRNG
{ .host = ELNRNG, .bionic = BIONIC_ELNRNG },
#endif
#ifdef EUNATCH
{ .host = EUNATCH, .bionic = BIONIC_EUNATCH },
#endif
#ifdef ENOCSI
{ .host = ENOCSI, .bionic = BIONIC_ENOCSI },
#endif
#ifdef EL2HLT
{ .host = EL2HLT, .bionic = BIONIC_EL2HLT },
#endif
#ifdef EBADE
{ .host = EBADE, .bionic = BIONIC_EBADE },
#endif
#ifdef EBADR
{ .host = EBADR, .bionic = BIONIC_EBADR },
#endif
#ifdef EXFULL
{ .host = EXFULL, .bionic = BIONIC_EXFULL },
#endif
#ifdef ENOANO
{ .host = ENOANO, .bionic = BIONIC_ENOANO },
#endif
#ifdef EBADRQC
{ .host = EBADRQC, .bionic = BIONIC_EBADRQC },
#endif
#ifdef EBADSLT
{ .host = EBADSLT, .bionic = BIONIC_EBADSLT },
#endif
#ifdef EBFONT
{ .host = EBFONT, .bionic = BIONIC_EBFONT },
#endif
#ifdef ENOSTR
{ .host = ENOSTR, .bionic = BIONIC_ENOSTR },
#endif
#ifdef ENODATA
{ .host = ENODATA, .bionic = BIONIC_ENODATA },
#endif
#ifdef ETIME
{ .host = ETIME, .bionic = BIONIC_ETIME },
#endif
#ifdef ENOSR
{ .host = ENOSR, .bionic = BIONIC_ENOSR },
#endif
#ifdef ENONET
{ .host = ENONET, .bionic = BIONIC_ENONET },
#endif
#ifdef ENOPKG
{ .host = ENOPKG, .bionic = BIONIC_ENOPKG },
#endif
#ifdef EREMOTE
{ .host = EREMOTE, .bionic = BIONIC_EREMOTE },
#endif
#ifdef ENOLINK
{ .host = ENOLINK, .bionic = BIONIC_ENOLINK },
#endif
#ifdef EADV
{ .host = EADV, .bionic = BIONIC_EADV },
#endif
#ifdef ESRMNT
{ .host = ESRMNT, .bionic = BIONIC_ESRMNT },
#endif
#ifdef ECOMM
{ .host = ECOMM, .bionic = BIONIC_ECOMM },
#endif
#ifdef EPROTO
{ .host = EPROTO, .bionic = BIONIC_EPROTO },
#endif
#ifdef EMULTIHOP
{ .host = EMULTIHOP, .bionic = BIONIC_EMULTIHOP },
#endif
#ifdef EDOTDOT
{ .host = EDOTDOT, .bionic = BIONIC_EDOTDOT },
#endif
#ifdef EBADMSG
{ .host = EBADMSG, .bionic = BIONIC_EBADMSG },
#endif
#ifdef EOVERFLOW
{ .host = EOVERFLOW, .bionic = BIONIC_EOVERFLOW },
#endif
#ifdef ENOTUNIQ
{ .host = ENOTUNIQ, .bionic = BIONIC_ENOTUNIQ },
#endif
#ifdef EBADFD
{ .host = EBADFD, .bionic = BIONIC_EBADFD },
#endif
#ifdef EREMCHG
{ .host = EREMCHG, .bionic = BIONIC_EREMCHG },
#endif
#ifdef ELIBACC
{ .host = ELIBACC, .bionic = BIONIC_ELIBACC },
#endif
#ifdef ELIBBAD
{ .host = ELIBBAD, .bionic = BIONIC_ELIBBAD },
#endif
#ifdef ELIBSCN
{ .host = ELIBSCN, .bionic = BIONIC_ELIBSCN },
#endif
#ifdef ELIBMAX
{ .host = ELIBMAX, .bionic = BIONIC_ELIBMAX },
#endif
#ifdef ELIBEXEC
{ .host = ELIBEXEC, .bionic = BIONIC_ELIBEXEC },
#endif
#ifdef EILSEQ
{ .host = EILSEQ, .bionic = BIONIC_EILSEQ },
#endif
#ifdef ERESTART
{ .host = ERESTART, .bionic = BIONIC_ERESTART },
#endif
#ifdef ESTRPIPE
{ .host = ESTRPIPE, .bionic = BIONIC_ESTRPIPE },
#endif
#ifdef EUSERS
{ .host = EUSERS, .bionic = BIONIC_EUSERS },
#endif
#ifdef ENOTSOCK
{ .host = ENOTSOCK, .bionic = BIONIC_ENOTSOCK },
#endif
#ifdef EDESTADDRREQ
{ .host = EDESTADDRREQ, .bionic = BIONIC_EDESTADDRREQ },
#endif
#ifdef EMSGSIZE
{ .host = EMSGSIZE, .bionic = BIONIC_EMSGSIZE },
#endif
#ifdef EPROTOTYPE
{ .host = EPROTOTYPE, .bionic = BIONIC_EPROTOTYPE },
#endif
#ifdef ENOPROTOOPT
{ .host = ENOPROTOOPT, .bionic = BIONIC_ENOPROTOOPT },
#endif
#ifdef EPROTONOSUPPORT
{ .host = EPROTONOSUPPORT, .bionic = BIONIC_EPROTONOSUPPORT },
#endif
#ifdef ESOCKTNOSUPPORT
{ .host = ESOCKTNOSUPPORT, .bionic = BIONIC_ESOCKTNOSUPPORT },
#endif
#ifdef EOPNOTSUPP
{ .host = EOPNOTSUPP, .bionic = BIONIC_EOPNOTSUPP },
#endif
#ifdef EPFNOSUPPORT
{ .host = EPFNOSUPPORT, .bionic = BIONIC_EPFNOSUPPORT },
#endif
#ifdef EAFNOSUPPORT
{ .host = EAFNOSUPPORT, .bionic = BIONIC_EAFNOSUPPORT },
#endif
#ifdef EADDRINUSE
{ .host = EADDRINUSE, .bionic = BIONIC_EADDRINUSE },
#endif
#ifdef EADDRNOTAVAIL
{ .host = EADDRNOTAVAIL, .bionic = BIONIC_EADDRNOTAVAIL },
#endif
#ifdef ENETDOWN
{ .host = ENETDOWN, .bionic = BIONIC_ENETDOWN },
#endif
#ifdef ENETUNREACH
{ .host = ENETUNREACH, .bionic = BIONIC_ENETUNREACH },
#endif
#ifdef ENETRESET
{ .host = ENETRESET, .bionic = BIONIC_ENETRESET },
#endif
#ifdef ECONNABORTED
{ .host = ECONNABORTED, .bionic = BIONIC_ECONNABORTED },
#endif
#ifdef ECONNRESET
{ .host = ECONNRESET, .bionic = BIONIC_ECONNRESET },
#endif
#ifdef ENOBUFS
{ .host = ENOBUFS, .bionic = BIONIC_ENOBUFS },
#endif
#ifdef EISCONN
{ .host = EISCONN, .bionic = BIONIC_EISCONN },
#endif
#ifdef ENOTCONN
{ .host = ENOTCONN, .bionic = BIONIC_ENOTCONN },
#endif
#ifdef ESHUTDOWN
{ .host = ESHUTDOWN, .bionic = BIONIC_ESHUTDOWN },
#endif
#ifdef ETOOMANYREFS
{ .host = ETOOMANYREFS, .bionic = BIONIC_ETOOMANYREFS },
#endif
#ifdef ETIMEDOUT
{ .host = ETIMEDOUT, .bionic = BIONIC_ETIMEDOUT },
#endif
#ifdef ECONNREFUSED
{ .host = ECONNREFUSED, .bionic = BIONIC_ECONNREFUSED },
#endif
#ifdef EHOSTDOWN
{ .host = EHOSTDOWN, .bionic = BIONIC_EHOSTDOWN },
#endif
#ifdef EHOSTUNREACH
{ .host = EHOSTUNREACH, .bionic = BIONIC_EHOSTUNREACH },
#endif
#ifdef EALREADY
{ .host = EALREADY, .bionic = BIONIC_EALREADY },
#endif
#ifdef EINPROGRESS
{ .host = EINPROGRESS, .bionic = BIONIC_EINPROGRESS },
#endif
#ifdef ESTALE
{ .host = ESTALE, .bionic = BIONIC_ESTALE },
#endif
#ifdef EUCLEAN
{ .host = EUCLEAN, .bionic = BIONIC_EUCLEAN },
#endif
#ifdef ENOTNAM
{ .host = ENOTNAM, .bionic = BIONIC_ENOTNAM },
#endif
#ifdef ENAVAIL
{ .host = ENAVAIL, .bionic = BIONIC_ENAVAIL },
#endif
#ifdef EISNAM
{ .host = EISNAM, .bionic = BIONIC_EISNAM },
#endif
#ifdef EREMOTEIO
{ .host = EREMOTEIO, .bionic = BIONIC_EREMOTEIO },
#endif
#ifdef EDQUOT
{ .host = EDQUOT, .bionic = BIONIC_EDQUOT },
#endif
#ifdef ENOMEDIUM
{ .host = ENOMEDIUM, .bionic = BIONIC_ENOMEDIUM },
#endif
#ifdef EMEDIUMTYPE
{ .host = EMEDIUMTYPE, .bionic = BIONIC_EMEDIUMTYPE },
#endif
#ifdef ECANCELED
{ .host = ECANCELED, .bionic = BIONIC_ECANCELED },
#endif
#ifdef ENOKEY
{ .host = ENOKEY, .bionic = BIONIC_ENOKEY },
#endif
#ifdef EKEYEXPIRED
{ .host = EKEYEXPIRED, .bionic = BIONIC_EKEYEXPIRED },
#endif
#ifdef EKEYREVOKED
{ .host = EKEYREVOKED, .bionic = BIONIC_EKEYREVOKED },
#endif
#ifdef EKEYREJECTED
{ .host = EKEYREJECTED, .bionic = BIONIC_EKEYREJECTED },
#endif
#ifdef EOWNERDEAD
{ .host = EOWNERDEAD, .bionic = BIONIC_EOWNERDEAD },
#endif
#ifdef ENOTRECOVERABLE
{ .host = ENOTRECOVERABLE, .bionic = BIONIC_ENOTRECOVERABLE, },
#endif
    };

    for(const auto &entry: errorMap) {
        if(entry.host == hostPlatformError)
            return entry.bionic;
    }

    return BIONIC_ENOSYS;

#else
    return hostPlatformError;
#endif
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
#include <sys/stat.h>

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


#include <Bionic/BionicSyscalls.h>

#include <errno.h>
#include <fcntl.h>

int translateErrno(int hostPlatformError) {
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
}

bool translateClockID(clockid_t &host, bionic_clockid_t bionic) {
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
}

int translateOpenFlag(int flag) {
    static_assert(
        O_ACCMODE == BIONIC_O_ACCMODE, "ACCMODE should match up");

    int oflag = O_BINARY | (flag & BIONIC_O_ACCMODE);

    if(flag & BIONIC_O_APPEND)
        oflag |= O_APPEND;

    if(flag & BIONIC_O_CREAT)
        oflag |= O_CREAT;

    if(flag & BIONIC_O_TRUNC)
        oflag |= O_TRUNC;

    if(flag & BIONIC_O_EXCL)
        oflag |= O_EXCL;

    if(flag & BIONIC_O_CLOEXEC)
        oflag |= O_NOINHERIT;

    if(flag & ~(BIONIC_O_ACCMODE | BIONIC_O_APPEND | BIONIC_O_CREAT | BIONIC_O_TRUNC | BIONIC_O_EXCL | BIONIC_O_CLOEXEC))
        return -1;

    return oflag;
}

int translateAccessMode(int mode) {
    int omode = 0;

    static_assert(BIONIC_F_OK == 0 && F_OK == 0, "Assuming that F_OK is zero");

    if(mode & BIONIC_R_OK)
        omode |= R_OK;

    if(mode & BIONIC_W_OK)
        omode |= W_OK;

    /*
     * No X_OK, Windows doesn't have it
     */

    return omode;
}

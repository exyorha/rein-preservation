#include "GlobalContext.h"
#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <ctime>
#include <cerrno>
#include <fcntl.h>

int plat_fstat(int fd, bionic_stat *statbuf) {
    struct stat64 native;

    auto result = ::fstat64(fd, &native);
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

bionic_off_t plat_lseek(int fildes, bionic_off_t offset, int whence) {
    auto result = lseek(fildes, offset, whence);
    if(result < 0)
        bionic_set_errno(translateErrno(errno));

    return result;
}

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

int plat_getdents64(unsigned int fd, bionic_dirent *dirp, unsigned int count) {
    fprintf(stderr, "plat_getdents64 is a stub on all platforms!\n");

    bionic_set_errno(BIONIC_ENOSYS);
    return -1;
}

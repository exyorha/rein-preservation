#include <Bionic/BionicSyscalls.h>

void translateTimespec(struct bionic_timespec *out, const struct timespec *in) {
    out->tv_sec = in->tv_sec;
    out->tv_nsec = in->tv_nsec;
}

void translateTimespecToNative(struct timespec *out, const struct bionic_timespec *in) {
    out->tv_sec = in->tv_sec;
    out->tv_nsec = in->tv_nsec;
}

void translateStat(const struct stat64 *native, struct bionic_stat *android) {
    android->st_dev = native->st_dev;
    android->st_ino = native->st_ino;
    android->st_mode = native->st_mode;
    android->st_nlink = native->st_nlink;
    android->st_uid = native->st_uid;
    android->st_gid = native->st_gid;
    android->st_rdev = native->st_rdev;
    android->st_size = native->st_size;
#ifdef _WIN32
    android->st_blksize = 512;
    android->st_blocks = (native->st_size + 511) / 512;
    android->st_atim.tv_sec = native->st_atime;
    android->st_atim.tv_nsec = 0;
    android->st_mtim.tv_sec = native->st_mtime;
    android->st_mtim.tv_nsec = 0;
    android->st_ctim.tv_sec = native->st_ctime;
    android->st_ctim.tv_nsec = 0;
#else
    android->st_blksize = native->st_blksize;
    android->st_blocks = native->st_blocks;
    translateTimespec(&android->st_atim, &native->st_atim);
    translateTimespec(&android->st_mtim, &native->st_mtim);
    translateTimespec(&android->st_ctim, &native->st_ctim);
#endif
}

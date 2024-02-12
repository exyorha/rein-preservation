#ifndef BIONIC_BIONIC_SYSCALLS_H
#define BIONIC_BIONIC_SYSCALLS_H

#include <Bionic/BionicABITypes.h>

int plat_openat(int fd, const char* path, int oflag, int mode);
int plat_fstat(int fd,  bionic_stat *statbuf);
int plat_fstatat(int fd, const char *path, struct bionic_stat *statbuf, int flag);
int plat_dup(int fd);
int plat_close(int fd);
bionic_ssize_t plat_read(int fd, void *dest, size_t size);
bionic_ssize_t plat_write(int fd, const void *dest, size_t size);
bionic_ssize_t plat_readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);
int plat_faccessat(int fd, const char *path, int amode, int flag);
int plat_renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);
int plat_unlinkat(int dirfd, const char *pathname, int flags);
int plat_mkdirat(int dirfd, const char *pathname, bionic_mode_t mode);
bionic_off_t plat_lseek(int fildes, bionic_off_t offset, int whence);

int plat_gettimeofday(bionic_timeval *tp, void *tzp);
int plat_clock_gettime(clockid_t clockid, bionic_timespec *tp);
int plat_clock_getres(clockid_t clockid, bionic_timespec *tp);
int plat_nanosleep(const bionic_timespec *rqtp, bionic_timespec *rmtp);

void *plat_mmap(void *addr, size_t len, int prot, int flags, int fildes, bionic_off_t off);
int plat_mprotect(void *addr, size_t len, int prot);
int plat_munmap(void *addr, size_t len);

int plat_uname(bionic_utsname *name);

#endif

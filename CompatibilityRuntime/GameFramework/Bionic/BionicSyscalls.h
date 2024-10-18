#ifndef BIONIC_BIONIC_SYSCALLS_H
#define BIONIC_BIONIC_SYSCALLS_H

#include <Bionic/BionicABITypes.h>

#include <sys/stat.h>

#include <time.h>

int plat_open(const char* path, int oflag, int mode);
int plat_fstat(int fd,  bionic_stat *statbuf);
int plat_stat(const char *path, struct bionic_stat *statbuf);
int plat_lstat(const char *path, struct bionic_stat *statbuf);
int plat_dup(int fd);
int plat_close(int fd);
bionic_ssize_t plat_read(int fd, void *dest, size_t size);
bionic_ssize_t plat_write(int fd, const void *dest, size_t size);
bionic_ssize_t plat_readlink(const char *pathname, char *buf, size_t bufsiz);
int plat_access(const char *path, int amode);
int plat_rename(const char *oldpath, const char *newpath);
int plat_rmdir(const char *pathname);
int plat_unlink(const char *pathname);
int plat_mkdir(const char *pathname, bionic_mode_t mode);
bionic_off_t plat_lseek(int fildes, bionic_off_t offset, int whence);
char *plat_getcwd(char *buf, size_t size);

int plat_clock_gettime(bionic_clockid_t clockid, bionic_timespec *tp);
int plat_clock_getres(bionic_clockid_t clockid, bionic_timespec *tp);
int plat_nanosleep(const bionic_timespec *rqtp, bionic_timespec *rmtp);

void *plat_mmap(void *addr, size_t len, int prot, int flags, int fildes, bionic_off_t off);
int plat_mprotect(void *addr, size_t len, int prot);
int plat_munmap(void *addr, size_t len);

int plat_getdents64(unsigned int fd, bionic_dirent *dirp, unsigned int count);

int translateErrno(int error);
bool translateClockID(clockid_t &host, bionic_clockid_t bionic);
int translateOpenFlag(int flag);
void translateTimespec(struct bionic_timespec *out, const struct timespec *in);
void translateTimespecToNative(struct timespec *out, const struct bionic_timespec *in) ;
void translateStat(const struct stat64 *native, struct bionic_stat *android);
int translateAccessMode(int mode);

#endif

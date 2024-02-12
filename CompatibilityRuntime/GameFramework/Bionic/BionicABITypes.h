#ifndef BIONIC_BIONIC_ABI_TYPES_H
#define BIONIC_BIONIC_ABI_TYPES_H

#include <cstdint>
#include <ELF/musl-elf.h>

#include <sys/types.h>

typedef int32_t bionic_pid_t;
typedef uint32_t bionic_mode_t;
typedef uint32_t bionic_nlink_t;
typedef uint64_t bionic_dev_t;
typedef uint64_t bionic_ino_t;
typedef uint32_t bionic_mode_t;
typedef uint32_t bionic_uid_t;
typedef uint32_t bionic_gid_t;
typedef int64_t bionic_off_t;
typedef uint64_t bionic_size_t;
typedef int64_t bionic_ssize_t;

struct bionic_timespec {
    int64_t tv_sec;
    int64_t tv_nsec;
};

struct BionicKernelArgumentBlock {
    int32_t argc;
    char **argv;
    char **envp;
    const Elf64_auxv_t *auxv;
    void **abort_message_ptr;
};

struct bionic_utsname {
    char  sysname   [65];
    char  nodename  [65];
    char  release   [65];
    char  version   [65];
    char  machine   [65];
    char  domainname[65];
};

struct bionic_stat {
  bionic_dev_t st_dev;
  bionic_ino_t st_ino;
  bionic_mode_t st_mode;
  bionic_nlink_t st_nlink;
  bionic_uid_t st_uid;
  bionic_gid_t st_gid;
  bionic_dev_t st_rdev;
  uint64_t __pad1;
  bionic_off_t st_size;
  int32_t st_blksize;
  int32_t __pad2;
  int64_t st_blocks;
  bionic_timespec st_atim;
  bionic_timespec st_mtim;
  bionic_timespec st_ctim;
  uint32_t __unused4;
  uint32_t __unused5;
};

struct bionic_dl_phdr_info {
  Elf64_Addr dlpi_addr;
  const char* dlpi_name;
  const Elf64_Phdr* dlpi_phdr;
  Elf64_Half dlpi_phnum;
};

struct bionic_timeval {
  int64_t tv_sec;
  int64_t tv_usec;
};

struct bionic_Dl_info;

#endif

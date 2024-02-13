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

typedef enum bionic_clockid : int32_t {
  BIONIC_CLOCK_REALTIME = 0,
  BIONIC_CLOCK_MONOTONIC = 1,
  BIONIC_CLOCK_PROCESS_CPUTIME_ID = 2,
  BIONIC_CLOCK_THREAD_CPUTIME_ID = 3,
  BIONIC_CLOCK_MONOTONIC_RAW = 4,
  BIONIC_CLOCK_REALTIME_COARSE = 5,
  BIONIC_CLOCK_MONOTONIC_COARSE = 6,
  BIONIC_CLOCK_BOOTTIME = 7,
  BIONIC_CLOCK_REALTIME_ALARM = 8,
  BIONIC_CLOCK_BOOTTIME_ALARM = 9,
  BIONIC_CLOCK_SGI_CYCLE = 10,
  BIONIC_CLOCK_CLOCK_TAI = 11
} bionic_clockid_t;

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

#define BIONIC_PROT_READ 0x1
#define BIONIC_PROT_WRITE 0x2
#define BIONIC_PROT_EXEC 0x4
#define BIONIC_PROT_SEM 0x8
#define BIONIC_PROT_NONE 0x0
#define BIONIC_PROT_GROWSDOWN 0x01000000
#define BIONIC_PROT_GROWSUP 0x02000000

#define BIONIC_MAP_SHARED 0x01
#define BIONIC_MAP_PRIVATE 0x02
#define BIONIC_MAP_TYPE 0x0f
#define BIONIC_MAP_FIXED 0x10
#define BIONIC_MAP_ANONYMOUS 0x20
#define BIONIC_MAP_UNINITIALIZED 0x0

#define BIONIC_MAP_FAILED ((void *)-1)

enum : int32_t {
#define __BIONIC_ERRDEF(name,value,msg) BIONIC_ ## name = value,
#include "BionicErrdefs.h"
#undef __BIONIC_ERRDEF
};

#endif

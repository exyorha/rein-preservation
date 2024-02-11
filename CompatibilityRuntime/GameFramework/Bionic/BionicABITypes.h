#ifndef BIONIC_BIONIC_ABI_TYPES_H
#define BIONIC_BIONIC_ABI_TYPES_H

#include <cstdint>
#include <elf.h>

struct BionicKernelArgumentBlock {
    int32_t argc;
    char **argv;
    char **envp;
    const Elf64_auxv_t *auxv;
    void **abort_message_ptr;
};

typedef int32_t bionic_pid_t;
typedef uint32_t bionic_mode_t;

struct bionic_utsname {
    char  sysname   [65];
    char  nodename  [65];
    char  release   [65];
    char  version   [65];
    char  machine   [65];
    char  domainname[65];
};
#endif

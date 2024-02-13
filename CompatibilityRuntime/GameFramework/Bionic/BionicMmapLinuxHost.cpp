#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <sys/mman.h>
#include <cerrno>

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

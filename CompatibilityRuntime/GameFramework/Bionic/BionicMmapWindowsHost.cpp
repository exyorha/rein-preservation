#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <windows.h>

#include "support.h"

static uint32_t queryAllocationGranulary();

static const uint32_t AllocationGranularity = queryAllocationGranulary();

static uint32_t queryAllocationGranulary() {
	SYSTEM_INFO sysinfo;

	GetSystemInfo(&sysinfo);

    return sysinfo.dwAllocationGranularity;
}

static DWORD convertProtection(int prot) {
    /*
     * We never set the execute permission, even if the application asks us to.
     */

    if(prot & BIONIC_PROT_WRITE) {
        return PAGE_READWRITE;
    } else if(prot & BIONIC_PROT_READ) {
        return PAGE_READONLY;
    } else {
        return PAGE_NOACCESS;
    }
}

void *plat_mmap(void *addr, size_t len, int prot, int flags, int fildes, bionic_off_t off) {
    if(len == 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return BIONIC_MAP_FAILED;
    }

    if(addr != nullptr) {
        panic("plat_mmap: mmap with a specified base address\n");
    }

    if(flags & BIONIC_MAP_FIXED) {
        panic("plat_mmap: mmap(MAP_FIXED)\n");
    }

    if(flags & BIONIC_MAP_ANONYMOUS) {
        auto allocated = VirtualAlloc(nullptr, len, MEM_RESERVE | MEM_COMMIT, convertProtection(prot));
        if(!allocated) {
            fprintf(stderr, "plat_mmap(MAP_ANONYMOUS): VirtualAlloc failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
            return BIONIC_MAP_FAILED;
        }

         return allocated;
    } else {
        panic("plat_mmap: file mmap\n");
    }
}

int plat_mprotect(void *addr, size_t len, int prot) {
    if(len == 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return -1;
    }

    DWORD oldProtect;

    if(!VirtualProtect(addr, len, convertProtection(prot), &oldProtect)) {
        fprintf(stderr, "plat_mprotect: VirtualProtect failed with %u\n", GetLastError());
        bionic_set_errno(BIONIC_ENOMEM);

        return -1;
    }

    return 0;
}

static bool releaseReservedOnlyPages(void *addr, size_t len) {

    auto start = (reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(addr) & ~static_cast<uintptr_t>(AllocationGranularity - 1)));
    auto end = (reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(addr) + len + AllocationGranularity - 1) & ~static_cast<uintptr_t>(AllocationGranularity - 1)));

    auto allocationPtr = start;
    while (allocationPtr < end) {
        MEMORY_BASIC_INFORMATION info;
        MEMORY_BASIC_INFORMATION info2;

        auto resultLength = VirtualQuery(allocationPtr, &info, sizeof(info));
        if (resultLength != sizeof(info))
            return false;

        auto regionEnd = std::min(end, static_cast<uint8_t*>(info.BaseAddress) + info.RegionSize);

        if (info.State == MEM_RESERVE &&
            info.BaseAddress == info.AllocationBase) {

            auto subiterator = allocationPtr;
            bool baseAddressChanged;
            do {
                resultLength = VirtualQuery(subiterator, &info2, sizeof(info2));
                if (resultLength != sizeof(info2))
                    return false;

                baseAddressChanged = info2.AllocationBase != info.AllocationBase;

                subiterator = static_cast<uint8_t*>(info.BaseAddress) + info.RegionSize;

            } while (!baseAddressChanged && info2.State == MEM_RESERVE);

            if (baseAddressChanged) {
                if (!VirtualFree(allocationPtr, 0, MEM_RELEASE))
                    return false;
            }
        }

        allocationPtr = regionEnd;
    }

    return true;
}

int plat_munmap(void *addr, size_t len) {
    if(len == 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return -1;
    }

    if(!VirtualFree(addr, len, MEM_DECOMMIT)) {
        fprintf(stderr, "plat_munmap: VirtualFree failed with %u\n", GetLastError());
        bionic_set_errno(BIONIC_ENOMEM);

        return -1;
    }

    releaseReservedOnlyPages(addr, len);

    return 0;
}

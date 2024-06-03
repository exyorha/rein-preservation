#include <Bionic/BionicSyscalls.h>
#include <Bionic/BionicCallouts.h>

#include <windows.h>
#include <Windows/WindowsHandle.h>
#include <io.h>

#include "Bionic/BionicABITypes.h"
#include "GlobalContext.h"
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
    if(len == 0 || (flags & (BIONIC_MAP_PRIVATE | BIONIC_MAP_SHARED)) == 0) {
        bionic_set_errno(BIONIC_EINVAL);
        return BIONIC_MAP_FAILED;
    }

    if(addr != nullptr && prot == BIONIC_PROT_NONE &&
        (flags & (BIONIC_MAP_ANONYMOUS | BIONIC_MAP_FIXED)) == (BIONIC_MAP_ANONYMOUS | BIONIC_MAP_FIXED)) {

        /*
         * This is Boehm GC requesting guard pages. Nothing needs to be done
         */

        return addr;
    }

    if(flags & BIONIC_MAP_FIXED) {
        panic("plat_mmap: mmap(MAP_FIXED)\n");
    }

    if(flags & BIONIC_MAP_ANONYMOUS) {
        void *allocated = nullptr;

        if(addr != nullptr) {
            allocated = VirtualAlloc(addr, len, MEM_RESERVE | MEM_COMMIT, convertProtection(prot));
        }

        if(!allocated)
            allocated = VirtualAlloc(nullptr, len, MEM_RESERVE | MEM_COMMIT, convertProtection(prot));

        if(!allocated) {
            fprintf(stderr, "plat_mmap(MAP_ANONYMOUS): VirtualAlloc failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
            return BIONIC_MAP_FAILED;
        }

         return allocated;
    } else {
        /*
         * Currently, any file mappings are mapped as MAP_SHARED, PROT_READ (only) mappings.
         * This should be sufficient.
         */
        auto fileHandle = reinterpret_cast<HANDLE>(_get_osfhandle(fildes));
        if(fileHandle == INVALID_HANDLE_VALUE) {
            bionic_set_errno(translateErrno(errno));
            return BIONIC_MAP_FAILED;
        }

        if(off & (AllocationGranularity - 1)) {
            fprintf(stderr, "plat_mmap(MAP_FILE): offset is not a multiple of Windows allocation granularity\n");
            bionic_set_errno(BIONIC_EINVAL);
            return BIONIC_MAP_FAILED;
        }

        LARGE_INTEGER fileSizeLI;
        if(!GetFileSizeEx(fileHandle, &fileSizeLI)) {
            fprintf(stderr, "plat_mmap(MAP_FILE): GetFileSizeEx failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
            return BIONIC_MAP_FAILED;
        }

        auto fileSize = fileSizeLI.QuadPart;

        if(off < 0) {
            bionic_set_errno(BIONIC_EINVAL);
            return BIONIC_MAP_FAILED;
        }

        uint64_t requiredFileSizeToSatifyRequest = off + len;

        /*
         * The application unhelpfully rounds up the mapping sizes to the page size,
         * however, that offends Windows if the file is shorter.
         */
        if(requiredFileSizeToSatifyRequest > ((fileSize + GlobalContext::PageSize - 1) & ~(GlobalContext::PageSize - 1))) {
            /*
             * Requesting more than 'the rest of the last page' of extra size is an error.
             */
            bionic_set_errno(BIONIC_EINVAL);
            return BIONIC_MAP_FAILED;
        }

        requiredFileSizeToSatifyRequest = std::min<uint64_t>(requiredFileSizeToSatifyRequest, fileSize);
        len = requiredFileSizeToSatifyRequest - off;

        HANDLE rawMapping = CreateFileMapping(fileHandle, nullptr, PAGE_READONLY,
                                                static_cast<DWORD>(requiredFileSizeToSatifyRequest >> 32),
                                                static_cast<DWORD>(requiredFileSizeToSatifyRequest),
                                                nullptr);
        if(!rawMapping) {
            fprintf(stderr, "plat_mmap(MAP_FILE): CreateFileMapping failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
            return BIONIC_MAP_FAILED;
        }

        WindowsHandle mapping(rawMapping);

        auto viewBase = MapViewOfFileEx(rawMapping, FILE_MAP_READ,
                                      static_cast<DWORD>(static_cast<uint64_t>(off) >> 32),
                                      static_cast<DWORD>(static_cast<uint64_t>(off)),
                                      len,
                                      addr);

        if(viewBase == nullptr) {
            fprintf(stderr, "plat_mmap(MAP_FILE): MapViewOfFile failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
            return BIONIC_MAP_FAILED;
        }

        return viewBase;
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

    MEMORY_BASIC_INFORMATION info;
    auto resultLength = VirtualQuery(addr, &info, sizeof(info));
    if(resultLength != sizeof(MEMORY_BASIC_INFORMATION)) {
        fprintf(stderr, "plat_munmap: VirtualQuery failed\n");
        bionic_set_errno(BIONIC_ENOMEM);

        return -1;
    }

    if(info.State != MEM_FREE && info.Type == MEM_MAPPED) {
        /*
         * This belongs to a mapped file. Just free the whole thing, since
         * that's what's usually asked.
         */

        if(!UnmapViewOfFile(info.AllocationBase)) {
            fprintf(stderr, "plat_munmap: UnmapViewOfFile failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);
        }

        return 0;
    } else {


        if(!VirtualFree(addr, len, MEM_DECOMMIT)) {
            fprintf(stderr, "plat_munmap: VirtualFree failed with %u\n", GetLastError());
            bionic_set_errno(BIONIC_ENOMEM);

            return -1;
        }

        releaseReservedOnlyPages(addr, len);

        return 0;
    }
}

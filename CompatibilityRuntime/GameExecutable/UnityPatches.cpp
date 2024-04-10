#include "UnityPatches.h"
#include "CommonUtilities.h"

#include <unity_stub.h>

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include "WindowsHelpers.h"
#else
#include <sys/mman.h>
#include <link.h>
#endif

#include <cinttypes>

struct PatchSite {
    uintptr_t patchVA;
    const unsigned char *patchContents;
    size_t size;
};

/*
 * These patches are only valid for this specific Unity version and variant.
 */
#ifdef _WIN32
#define EXPECTED_UNITY_VERSION "2019.4.29f1 'win64_development_il2cpp' variant"
#define EXPECTED_UNITY_CRC32 UINT32_C(0xfe4782a0)

/*
 * Kills the platform compatibility check in SerializedFile::ReadMetadata<1>: 'always compatible'.
 */
static const unsigned char patch_11d0e1c[]{
    0x90, 0x90
};

/*
 * Kills the platform compatibility check in SerializedFile::ReadMetadata<0>: 'always compatible'.
 */
static const unsigned char patch_11d1dda[]{
    0x90, 0x90
};

static const PatchSite UnityPlayerPatchSites[] = {
    { 0x11d0e1c, patch_11d0e1c, sizeof(patch_11d0e1c) },
    { 0x11d1dda, patch_11d1dda, sizeof(patch_11d1dda) },
};
#else
#define EXPECTED_UNITY_VERSION "2019.4.29f1 'linux64_withgfx_development_il2cpp' variant"
#define EXPECTED_UNITY_CRC32 UINT32_C(0xecae9fda)

// SerializedFile::ReadMetadata<false>: allow all platforms
static const unsigned char patch_13e3a0b[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

// IsRendererSupported: allow all renderers
static const unsigned char patch_1408950[] = { 0xb8, 0x01, 0x00, 0x00, 0x00, 0xc3 };

static const PatchSite UnityPlayerPatchSites[] = {
    { 0x13e3a0b, patch_13e3a0b, sizeof(patch_13e3a0b) },
    { 0x1408950, patch_1408950, sizeof(patch_1408950) },
};
#endif

static uint32_t crc32Small(const unsigned char *data, size_t size, uint32_t previous = 0) {
    static const uint32_t table[]{
        0x00000000,0x1DB71064,0x3B6E20C8,0x26D930AC,0x76DC4190,0x6B6B51F4,0x4DB26158,0x5005713C,
        0xEDB88320,0xF00F9344,0xD6D6A3E8,0xCB61B38C,0x9B64C2B0,0x86D3D2D4,0xA00AE278,0xBDBDF21C
    };

    uint32_t crc = ~previous;

    auto ptr = data;
    auto limit = ptr + size;
    while(ptr < limit) {
        auto byte = *ptr++;

        crc = table[(crc ^ byte) & 15] ^ (crc >> 4);
        byte >>= 4;
        crc = table[(crc ^ byte) & 15] ^ (crc >> 4);
    }

    return ~crc;
}

static bool calculateFileCRC32(const std::filesystem::path &filename, uint32_t &outputCRC) {
    struct FileDescriptorHolder {
        int fd = -1;

        ~FileDescriptorHolder() {
            if(fd >= 0) {
                close(fd);
            }
        }

    } fdHolder;

    struct WorkBufferHolder {
        unsigned char *buf = nullptr;

        ~WorkBufferHolder() {
            free(buf);
        }

    } bufHolder;

#ifdef _WIN32
    fdHolder.fd = _wopen(filename.c_str(), O_RDONLY | O_BINARY);
#else
    fdHolder.fd = open(filename.c_str(), O_RDONLY | O_CLOEXEC | O_NOCTTY);
#endif
    if(fdHolder.fd < 0)
        return false;

    static constexpr size_t ChunkSize = 128 * 1024;

    bufHolder.buf = static_cast<unsigned char *>(malloc(ChunkSize));
    if(!bufHolder.buf)
        return false;

    uint32_t crc = 0;

    ssize_t bytesRead;
    while(true) {
        do {
            bytesRead = read(fdHolder.fd, bufHolder.buf, ChunkSize);
        } while(bytesRead < 0 && errno == EINTR);

        if(bytesRead < 0)
            return false;

        if(bytesRead == 0)
            break;

        crc = crc32Small(bufHolder.buf, static_cast<size_t>(bytesRead), crc);
    }

    outputCRC = crc;

    return true;
}

static bool applyPatches(const PatchSite *patches, size_t patchCount, intptr_t displacement) {
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    uintptr_t pageSize = info.dwPageSize;
#else
    auto result = sysconf(_SC_PAGE_SIZE);
    if(result < 0)
        return false;

    uintptr_t pageSize = static_cast<size_t>(result);
#endif

    class PageProtectionManager {
    public:
        PageProtectionManager(size_t pageSize) : pageSize(pageSize) {

        }

        size_t pageSize;

        bool allowWritesToPage(uintptr_t address) {
            if(std::find_if(m_pagesWithAdjustedPermissions.begin(), m_pagesWithAdjustedPermissions.end(),
                [address](const auto &existing) { return existing.first == address; }) !=
                m_pagesWithAdjustedPermissions.end())
                return true;

#ifdef _WIN32
            DWORD oldPermissions;
            if(!VirtualProtect(reinterpret_cast<void *>(address), pageSize, PAGE_READWRITE, &oldPermissions)) {
                return false;
            }
#else
            uint32_t oldPermissions = PROT_READ | PROT_EXEC;

            if(mprotect(reinterpret_cast<void *>(address), pageSize, PROT_READ | PROT_WRITE) < 0)
                return false;
#endif

            m_pagesWithAdjustedPermissions.emplace_back(address, oldPermissions);

            return true;
        }

        bool restorePermissions() {
            for(auto page: m_pagesWithAdjustedPermissions) {
#ifdef _WIN32
                DWORD oldPermissions;
                if(!VirtualProtect(reinterpret_cast<void *>(page.first), pageSize, page.second, &oldPermissions)) {
                    return false;
                }
#else
                if(mprotect(reinterpret_cast<void *>(page.first), pageSize, page.second) < 0)
                    return false;
#endif
            }

            return true;
        }

    private:
        std::vector<std::pair<uintptr_t, uint32_t>> m_pagesWithAdjustedPermissions;
    } protectionManager(pageSize);

    auto patchLimit = patches + patchCount;
    for(auto patch = patches; patch < patchLimit; patch++) {
        auto address = patch->patchVA + displacement;

        uintptr_t startingPage = address & ~(pageSize - 1);
        uintptr_t limitingPage = (address + patch->size + pageSize - 1) & ~(pageSize - 1);

        for(auto pageToAdjust = startingPage; pageToAdjust < limitingPage; pageToAdjust += pageSize) {
            if(!protectionManager.allowWritesToPage(pageToAdjust))
                return false;
        }

        memcpy(reinterpret_cast<void *>(patch->patchVA + displacement), patch->patchContents, patch->size);
    }

    return protectionManager.restorePermissions();
}

bool applyUnityPatches() {


    uint32_t crc = 0;
    void *unityBaseAddress;

#ifdef _WIN32
    auto unityModule = GetModuleHandle(L"UnityPlayer.dll");
    if(unityModule == nullptr) {
        fprintf(stderr, "Unable to locate the loaded UnityPlayer.\n");
        return false;
    }

    unityBaseAddress = reinterpret_cast<void *>(unityModule);

    auto unityPath = getPathToModule(unityModule);

#else
    const char *unityName = "UnityPlayer.so";

    Dl_info info;

    if(!dladdr(reinterpret_cast<void *>(&PlayerMain), &info)) {
        fprintf(stderr, "Unable to locate the loaded %s.\n", unityName);
        return false;
    }

    auto basename = strrchr(info.dli_fname, '/');
    if(basename == nullptr) {
        basename = info.dli_fname;
    } else {
        ++basename;
    }

    if(!stringsEqualIgnoreCaseAsciiOnly(basename, unityName)) {
        fprintf(stderr, "PlayerMain is not provided by '%s', but by '%s'.\n", unityName, info.dli_fname);
        return false;
    }

    unityBaseAddress = reinterpret_cast<void *>(info.dli_fbase);

    const std::filesystem::path &unityPath = info.dli_fname;

#endif
    if(!calculateFileCRC32(unityPath, crc)) {
        fprintf(stderr, "Failed to calculate the checksum of '%s': %s\n",
                unityPath.string().c_str(), strerror(errno));
        return false;
    }

    if(crc != EXPECTED_UNITY_CRC32) {
        fprintf(stderr, "The CRC-32 of the Unity player runtime, which was calculated to be 0x%08" PRIx32 "\n"
                        "didn't match the expected value, 0x%08" PRIx32 ".\n"
                        "\n"
                        "We currently rely on patching the Unity binary to be able to use the OpenGL ES\n"
                        "renderer on the desktop, and to be able to use the assets targeting Android\n"
                        "on other platforms. As such, the only Unity version supported is\n"
                        "%s.\n"
                        "\n"
                        "Any other version would be incompatible unless the patches are adjusted.\n"
                        "\n"
                        "Use the compatible version listed above, or defeat this check at your own peril.\n",
                crc, EXPECTED_UNITY_CRC32, EXPECTED_UNITY_VERSION);

        return false;
    }

    if(!applyPatches(
        UnityPlayerPatchSites,
        sizeof(UnityPlayerPatchSites) / sizeof(UnityPlayerPatchSites[0]),
        reinterpret_cast<intptr_t>(unityBaseAddress))) {

        fprintf(stderr, "Failed to apply the in-memory executable patches to the Unity player\n");
        return false;
    }

    return true;
}

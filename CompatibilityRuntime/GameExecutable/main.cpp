#include <unity_stub.h>
#include <translator_api.h>

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cinttypes>
#include <vector>
#include <algorithm>
#include <sys/mman.h>
#include <link.h>
#include <fcntl.h>
#include <unistd.h>
#include <array>

#include <gameserver_api.h>

#include "Il2CppUtilities.h"

struct PatchSite {
    uintptr_t patchVA;
    const unsigned char *patchContents;
    size_t size;
};

static Gameserver *gameserverInstance;

/*
 * These patches are only valid for this specific Unity version and variant.
 */
#define EXPECTED_UNITY_VERSION "2019.4.29f1 'linux64_withgfx_development_il2cpp' variant"
#define EXPECTED_UNITY_CRC32 UINT32_C(0xecae9fda)
static const unsigned char patch_13e3a0b[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
static const unsigned char patch_1408950[] = { 0xb8, 0x01, 0x00, 0x00, 0x00, 0xc3 };
static const unsigned char patch_14132f8[] = { 0x20 };
static const unsigned char patch_1413305[] = { 0x04 };
static const unsigned char patch_1413310[] = { 0xbe, 0x03, 0x00, 0x00, 0x00, 0x90, 0x90 };
static const unsigned char patch_1413323[] = { 0xbe, 0x02, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 };
static const unsigned char patch_1413347[] = { 0x03 };
static const unsigned char patch_141334b[] = { 0xb8, 0x03, 0x00, 0x00, 0x00, 0xbe, 0x02, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0xba, 0x01, 0x00, 0x00,
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

/*
 * Patches a check in ScreenManagerLinux::HandleResolutionChange to make it use the 'downscaled FBO' code path with the GLES renderer (two sites)
 */
static const unsigned char patch_1a857a3[] = { 11 };
static const unsigned char patch_1a857ca[] = { 11 };
/*
 * Similar check in ScreenManagerLinux::SetResolutionImmediate.
 */
static const unsigned char patch_1a85b39[] = { 11 };
static const unsigned char patch_1a85d01[] = { 11 };

static const PatchSite UnityPlayerPatchSites[] = {
    { 0x13e3a0b, patch_13e3a0b, sizeof(patch_13e3a0b) },
    { 0x1408950, patch_1408950, sizeof(patch_1408950) },
    { 0x14132f8, patch_14132f8, sizeof(patch_14132f8) },
    { 0x1413305, patch_1413305, sizeof(patch_1413305) },
    { 0x1413310, patch_1413310, sizeof(patch_1413310) },
    { 0x1413323, patch_1413323, sizeof(patch_1413323) },
    { 0x1413347, patch_1413347, sizeof(patch_1413347) },
    { 0x141334b, patch_141334b, sizeof(patch_141334b) },
    { 0x1a857a3, patch_1a857a3, sizeof(patch_1a857a3) },
    { 0x1a857ca, patch_1a857ca, sizeof(patch_1a857ca) },
    { 0x1a85b39, patch_1a85b39, sizeof(patch_1a85b39) },
    { 0x1a85d01, patch_1a85d01, sizeof(patch_1a85d01) },
};


static bool stringsEqualIgnoreCaseAsciiOnly(const char *a, const char *b) {
    char chB;

    while(*a) {
        char chA = *a++;
        chB = *b++;

        if(chA >= 'A' && chA <= 'Z')
            chA = chA - 'A' + 'a';

        if(chB >= 'A' && chB <= 'Z')
            chB = chB - 'A' + 'a';

        if(chA != chB)
            return false;
    }

    return *b == 0;

}

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

static bool calculateFileCRC32(const char *filename, uint32_t &outputCRC) {
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

    fdHolder.fd = open(filename, O_RDONLY | O_CLOEXEC | O_NOCTTY);
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
    auto result = sysconf(_SC_PAGE_SIZE);
    if(result < 0)
        return false;

    auto pageSize = static_cast<size_t>(result);

    class PageProtectionManager {
    public:
        PageProtectionManager(size_t pageSize) : pageSize(pageSize) {

        }

        size_t pageSize;

        bool allowWritesToPage(uintptr_t address) {
            if(std::find(m_pagesWithAdjustedPermissions.begin(), m_pagesWithAdjustedPermissions.end(), address) !=
                m_pagesWithAdjustedPermissions.end())
                return true;

            if(mprotect(reinterpret_cast<void *>(address), pageSize, PROT_READ | PROT_WRITE) < 0)
                return false;

            m_pagesWithAdjustedPermissions.emplace_back(address);

            return true;
        }

        bool restorePermissions() {
            for(auto page: m_pagesWithAdjustedPermissions) {
                if(mprotect(reinterpret_cast<void *>(page), pageSize, PROT_READ | PROT_EXEC) < 0)
                    return false;
            }

            return true;
        }

    private:
        std::vector<uintptr_t> m_pagesWithAdjustedPermissions;
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

static void com_adjust_sdk_Adjust_start(Il2CppObject *config, void (*original)(Il2CppObject *config)) {
    printf("com.adjust.sdk.Adjust::start diversion called, interposed function: %p\n", original);
}

static void Subsystem_AdjustSDK_AdjustUtility_TrackEvent(Il2CppString *event, void (*original)(Il2CppString *event)) {
    printf("Subsystem.AdjustSDK.AdjustUtility.TrackEvent diversion called, interposed function: %p, event: %p\n", original,
           event);
}

static Il2CppString *com_adjust_sdk_Adjust_getAdid(Il2CppString *(*original)(void)) {
    printf("com.adjust.sdk.Adjust::getAdid diversion called\n");

    return il2cpp_string_new("");
}

static Il2CppString *Octo_Util_FileUtil_GetAndroidOctoRoot(Il2CppString *(*original)(void)) {
    printf("Octo.dll::Octo.Util.FileUtil::GetAndroidOctoRoot diversion called\n");

    return il2cpp_string_new("/home/reki/rein/octoroot");
}

static void Firebase_Crashlytics_Crashlytics_Log(Il2CppString *string, void (*original)(Il2CppString *string)) {
    printf("Firebase-Crashlytics.dll::Firebase.Crashlytics.Crashlytics::Log diversion called, string: %s\n",
           stringToUtf8(string).c_str());
}
static void Firebase_Crashlytics_Crashlytics_SetUserId(Il2CppString *string, void (*original)(Il2CppString *string)) {
    printf("Firebase-Crashlytics.dll::Firebase.Crashlytics.Crashlytics::SetUserId diversion called, string: %s\n",
           stringToUtf8(string).c_str());
}

static Il2CppArray *Dark_StateMachine_HandleNet_Encrypt(Il2CppObject *this_, Il2CppArray *data,
                                                            Il2CppArray *(*original)(Il2CppObject *this_, Il2CppArray *data)) {

    (void)this_;
    (void)original;
    return data;
}

static Il2CppArray *Dark_StateMachine_HandleNet_Decrypt(Il2CppObject *this_, Il2CppArray *data,
                                                            Il2CppArray *(*original)(Il2CppObject *this_, Il2CppArray *data)) {

    (void)this_;
    (void)original;
    return data;
}

static void SafetyNet_SafetyNet_Attest(Il2CppString *parameter1, Il2CppString *parameter2, Il2CppObject *attestListener,
                                       void (*original)(Il2CppString *, Il2CppString *, Il2CppObject *)) {

    printf("SafetyNet.Attest: '%s', '%s', listener: %p\n", stringToUtf8(parameter1).c_str(), stringToUtf8(parameter2).c_str(), attestListener);

    if(!attestListener) {
        fprintf(stderr, "SafetyNet.Attest: null listener\n");
        return;
    }

    auto listenerClass = il2cpp_object_get_class(attestListener);
    auto resultMethod = il2cpp_class_get_method_from_name(listenerClass, "onResult", 1);
    if(!resultMethod) {
        fprintf(stderr, "SafetyNet.Attest: could not find the onResult method in the passed listener");
        return;
    }

    auto resultString = il2cpp_string_new("");

    std::array<Il2CppObject *, 1> params;
    params[0] = reinterpret_cast<Il2CppObject *>(resultString);

    Il2CppException *exception = nullptr;
    il2cpp_runtime_invoke_convert_args(resultMethod, attestListener, params.data(), params.size(), &exception);

    if(exception) {
        char buf[256];
        il2cpp_format_exception(exception, buf, sizeof(buf));
        fprintf(stderr, "SafetyNet.Attest: the callback has failed with exception: %.*s\n", static_cast<int>(sizeof(buf)), buf);
        return;
    }
}

static Il2CppString *DeviceUtil_DeviceUtil_GetAcs(Il2CppString (*original)(void)) {
    printf("DeviceUtil.GetAcs\n");

    return il2cpp_string_new("");
}

static bool DeviceUtil_DeviceUtil_GetPer(bool (*original)(void)) {
    printf("DeviceUtil.GetPer\n");

    return false;
}

static bool DeviceUtil_DeviceUtil_GetImu(bool (*original)(void)) {
    printf("DeviceUtil.GetImu\n");

    return false;
}

static bool DeviceUtil_DeviceUtil_GetIr(bool (*original)(void)) {
    printf("DeviceUtil.GetIr\n");

    return false;
}

static bool DeviceUtil_DeviceUtil_GetIda(bool (*original)(void)) {
    printf("DeviceUtil.GetIda\n");

    return false;
}

/*
 * Downsizes the gRPC thread pool
static void Grpc_Core_Internal_GrpcThreadPool_ctor(Il2CppObject *this_, Il2CppObject *environment, int32_t poolSize, int32_t queueCount,
                                                    bool inlineHandlers,
                                                    void (*original)(Il2CppObject *this_, Il2CppObject *environment, int32_t poolSize, int32_t queueCount,
                                                    bool inlineHandlers)) {
    printf("Assembly-CSharp.dll::Grpc.Core.Internal.GrpcThreadPool::.ctor: this %p, environment %p, pool size %d, queue count %d, inline handlers %d\n",
            this_, environment, poolSize, queueCount, inlineHandlers);

    original(this_, environment, 1, 1, true);
}
 */

static void postInitialize() {
    printf("--------- GameExecutable: il2cpp is now initialized, installing managed code diversions\n");

    translator_divert_method("Assembly-CSharp.dll::com.adjust.sdk.Adjust::start",
                             com_adjust_sdk_Adjust_start);

    translator_divert_method("Assembly-CSharp.dll::Subsystem.AdjustSDK.AdjustUtility::TrackEvent",
                             Subsystem_AdjustSDK_AdjustUtility_TrackEvent);

    translator_divert_method("Assembly-CSharp.dll::com.adjust.sdk.Adjust::getAdid",
                             com_adjust_sdk_Adjust_getAdid);

    translator_divert_method("Octo.dll::Octo.Util.FileUtil::GetAndroidOctoRoot",
                             Octo_Util_FileUtil_GetAndroidOctoRoot);

    translator_divert_method("Firebase.Crashlytics.dll::Firebase.Crashlytics.Crashlytics::Log",
                             Firebase_Crashlytics_Crashlytics_Log);

    translator_divert_method("Firebase.Crashlytics.dll::Firebase.Crashlytics.Crashlytics::SetUserId",
                             Firebase_Crashlytics_Crashlytics_SetUserId);

    /*
     * It's easier to stub Encrypt/Decrypt out than to implement encryption hooks in the gRPC server.
     */
    translator_divert_method("Assembly-CSharp.dll::Dark.StateMachine.HandleNet::Encrypt",
                             Dark_StateMachine_HandleNet_Encrypt);

    translator_divert_method("Assembly-CSharp.dll::Dark.StateMachine.HandleNet::Decrypt",
                             Dark_StateMachine_HandleNet_Decrypt);

    translator_divert_method("Assembly-CSharp.dll::SafetyNet.SafetyNet::Attest",
                             SafetyNet_SafetyNet_Attest);

    translator_divert_method("Assembly-CSharp.dll::DeviceUtil.DeviceUtil::GetAcs",
                             DeviceUtil_DeviceUtil_GetAcs);

    translator_divert_method("Assembly-CSharp.dll::DeviceUtil.DeviceUtil::GetPer",
                             DeviceUtil_DeviceUtil_GetPer);

    translator_divert_method("Assembly-CSharp.dll::DeviceUtil.DeviceUtil::GetImu",
                             DeviceUtil_DeviceUtil_GetImu);

    translator_divert_method("Assembly-CSharp.dll::DeviceUtil.DeviceUtil::GetIr",
                             DeviceUtil_DeviceUtil_GetIr);

    translator_divert_method("Assembly-CSharp.dll::DeviceUtil.DeviceUtil::GetIda",
                             DeviceUtil_DeviceUtil_GetIda);
/*
 * Downsizes the gRPC thread pool
    translator_divert_method("Assembly-CSharp-firstpass.dll::Grpc.Core.Internal.GrpcThreadPool::.ctor",
                             Grpc_Core_Internal_GrpcThreadPool_ctor);
*/
#if 0
    auto getEnabledInternal = reinterpret_cast<bool (*)(void)>(translator_resolve_native_icall("UnityEngine.Analytics.Analytics::get_enabledInternal"));
    printf("getEnabledInternal: %p\n", getEnabledInternal);

    printf("Analytics enabled: %d\n", getEnabledInternal());
#endif
}

static void grpcRedirection(TranslatorGrpcChannelSetup *setup) {
    printf("gRPC redirection: creating a channel to %s, attributes %p, credentials %p, secure %d\n",
           setup->target, setup->args, setup->creds, setup->secure);

#if 1
    setup->target = "127.0.0.1:8087";
    setup->creds = nullptr;
    setup->secure = 0;
#else

    if(!gameserverInstance) {
        fprintf(stderr, "the game server is not running\n");
        abort();
    }

    setup->channel = gameserver_open_in_process_api_channel(gameserverInstance, setup->args);

    if(!setup->channel) {
        fprintf(stderr, "gameserver_open_in_process_api_channel has failed\n");
        abort();
    }
#endif
}

static int gameMain(int argc, char **argv) {
    auto gameserver = makeGameServer();

    if(!gameserver) {
        fprintf(stderr, "failed to initialize the built-in game server\n");
        return 1;
    }

    gameserverInstance = gameserver.get();

    int result = PlayerMain(argc, argv);

    gameserverInstance = nullptr;

    return result;
}

int main(int argc, char **argv) {
    const char *unityName = "UnityPlayer.so";

    Dl_info info;

    if(!dladdr(reinterpret_cast<void *>(&PlayerMain), &info)) {
        fprintf(stderr, "Unable to locate the loaded %s.\n", unityName);
        return 1;
    }

    auto basename = strrchr(info.dli_fname, '/');
    if(basename == nullptr) {
        basename = info.dli_fname;
    } else {
        ++basename;
    }

    if(!stringsEqualIgnoreCaseAsciiOnly(basename, unityName)) {
        fprintf(stderr, "PlayerMain is not provided by '%s', but by '%s'.\n", unityName, info.dli_fname);
        return 1;
    }

    uint32_t crc;
    if(!calculateFileCRC32(info.dli_fname, crc)) {
        fprintf(stderr, "Failed to calculate the checksum of '%s': %s\n",
                info.dli_fname, strerror(errno));
        return 1;
    }

    if(crc != EXPECTED_UNITY_CRC32) {
        fprintf(stderr, "The CRC-32 of the Unity player runtime, '%s',\n"
                        "which was calculated to be 0x%08" PRIx32 ", didn't match the expected value,\n"
                        "0x%08" PRIx32 ".\n"
                        "\n"
                        "We currently rely on patching the Unity binary to be able to use the OpenGL ES\n"
                        "renderer on the desktop, and to be able to use the assets targeting Android\n"
                        "on other platforms. As such, the only Unity version supported is\n"
                        "%s.\n"
                        "\n"
                        "Any other version would be incompatible unless the patches are adjusted.\n"
                        "\n"
                        "Use the compatible version listed above, or defeat this check at your own peril.\n",
                info.dli_fname,
                crc, EXPECTED_UNITY_CRC32, EXPECTED_UNITY_VERSION);

        return 1;
    }

    if(!applyPatches(
        UnityPlayerPatchSites,
        sizeof(UnityPlayerPatchSites) / sizeof(UnityPlayerPatchSites[0]),
        reinterpret_cast<intptr_t>(info.dli_fbase))) {

        fprintf(stderr, "Failed to apply the in-memory executable patches to the Unity player\n");
        return 1;
    }

    translator_set_post_initialize_callback(postInitialize);
    translator_set_grpc_redirection_callback(grpcRedirection);

    return translator_main(argc, argv, gameMain);
}

#include <filesystem>
#include <unity_stub.h>
#include <translator_api.h>

#include <cstdio>
#include <array>
#include <random>
#include <algorithm>

#include "UnityPatches.h"
#include "Il2CppUtilities.h"
#include "Input.h"
#include "OctoContentStorage.h"
#include "Octo.h"
#include "FastAES.h"

#ifdef _WIN32
#include "WindowsHelpers.h"
#include "GLES/WGL/WGLHooking.h"
#else
#include "GLES/Shim/GLESContextShim.h"

#include <GLES/SDL/SDLWrapper.h>
#endif

OctoContentStorage *contentStorageInstance;

static void com_adjust_sdk_Adjust_start(Il2CppObject *config, void (*original)(Il2CppObject *config)) {
    printf("com.adjust.sdk.Adjust::start\n");
}

static void com_adjust_sdk_Adjust_OnApplicationPause(Il2CppObject *this_, bool paused, void *original) {
    printf("com.adjust.sdk.Adjust::OnApplicationPause: paused: %d\n", paused);
}

static void Subsystem_AdjustSDK_AdjustUtility_TrackEvent(Il2CppString *event, void (*original)(Il2CppString *event)) {
    printf("Subsystem.AdjustSDK.AdjustUtility.TrackEvent: event: %s\n", stringToUtf8(event).c_str());
}

static Il2CppString *com_adjust_sdk_Adjust_getAdid(Il2CppString *(*original)(void)) {
    printf("com.adjust.sdk.Adjust::getAdid diversion called\n");

    return il2cpp_string_new("");
}

static void Firebase_Crashlytics_Crashlytics_Log(Il2CppString *string, void (*original)(Il2CppString *string)) {
    printf("Firebase.Crashlytics.Crashlytics::Log: %s\n",
           stringToUtf8(string).c_str());
}
static void Firebase_Crashlytics_Crashlytics_SetUserId(Il2CppString *string, void (*original)(Il2CppString *string)) {
    printf("Firebase.Crashlytics.Crashlytics::SetUserId: %s\n",
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
 */
static void Grpc_Core_Internal_GrpcThreadPool_ctor(Il2CppObject *this_, Il2CppObject *environment, int32_t poolSize, int32_t queueCount,
                                                    bool inlineHandlers,
                                                    void (*original)(Il2CppObject *this_, Il2CppObject *environment, int32_t poolSize, int32_t queueCount,
                                                    bool inlineHandlers)) {
    printf("Assembly-CSharp.dll::Grpc.Core.Internal.GrpcThreadPool::.ctor: this %p, environment %p, pool size %d, queue count %d, inline handlers %d\n",
            this_, environment, poolSize, queueCount, inlineHandlers);

    original(this_, environment, 1, 1, true);
}

static void Firebase_Analytics_FirebaseAnalytics_cctor(void (*original)()) {
    printf("Firebase.Analytics.dll::Firebase.Analytics.FirebaseAnalytics::.cctor\n");
}

static void Firebase_Analytics_FirebaseAnalytics_SetAnalyticsCollectionEnabled(bool enabled, void (*original)(bool enabled)) {
    printf("Firebase.Analytics.dll::Firebase.Analytics.FirebaseAnalytics::SetAnalyticsCollectionEnabled(%d)\n", enabled);
}

/*
 * Since we never actually download anything, the storage is always enough.
 */
static bool Framework_Network_Download_AssetDownloader_IsStorageEnough(Il2CppObject *this_, int64_t size, void *original) {
    printf("Framework.Network.Download.AssetDownloader.IsStorageEnough(%ld)\n", size);

    return true;
}

static bool RenderHeads_Media_AVProVideo_AndroidMediaPlayer_InitializePlatform(void *original) {
    printf("RenderHeads.Media.AVProVideo.AndroidMediaPlayer::InitializePlatform\n");

    return false;
}

/*
 * Stubbing out this avoids initializing the whole Firebase stuff, including the native library.
 */
static void Adam_Framework_Notification_Notification_SetupNotification(void *original) {
    printf("Adam.Framework.Notification.Notification::SetupNotification\n");
}

/*
 * Do not allow disabling logging; keep logging enabled.
 */
static void UnityEngine_Logger_set_logEnabled(Il2CppObject *instance, bool enabled, void (*original)(Il2CppObject *instance, bool enabled)) {
    enabled = true;

    original(instance, enabled);
}

#ifdef _WIN32
/*
 * To make things easier on Windows, replace the system random provider.
 * The default one uses /dev/urandom.
 */
static void System_Security_Cryptography_RNGCryptoServiceProvider_Check(Il2CppObject *this_, void *original) {
    printf("System.Security.Cryptography.RNGCryptoServiceProvider::Check\n");
}

static void System_Security_Cryptography_RNGCryptoServiceProvider_GetBytes(Il2CppObject *this_, Il2CppArray *out, void *original) {
    (void)this_;
    (void)original;


    int32_t dataLength = il2cpp_array_length(out);
    auto arrayHeaderSize = il2cpp_array_object_header_size();
    auto data = reinterpret_cast<uint8_t *>(out) + arrayHeaderSize;

    printf("System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes(%p, %d)\n", data, dataLength);

    using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
    std::generate(data, data + dataLength, random_bytes_engine());

}
#endif

/*
 * Avoids a JNI callout to java/util/TimeZone.
 */
static Il2CppString *Dark_Localization_LocalizeTime_GetLocalTimeZoneId(Il2CppString *(*original)()) {
    return stringFromUtf8("Etc/UTC");
}

static void postInitialize() {
    printf("--------- GameExecutable: il2cpp is now initialized, installing managed code diversions\n");

    translator_divert_method("Assembly-CSharp.dll::com.adjust.sdk.Adjust::start",
                             com_adjust_sdk_Adjust_start);

#if 0
    translator_divert_method("Assembly-CSharp.dll::com.adjust.sdk.Adjust::OnApplicationPause",
                             com_adjust_sdk_Adjust_OnApplicationPause);
#endif

    translator_divert_method("Assembly-CSharp.dll::Subsystem.AdjustSDK.AdjustUtility::TrackEvent",
                             Subsystem_AdjustSDK_AdjustUtility_TrackEvent);

    translator_divert_method("Assembly-CSharp.dll::com.adjust.sdk.Adjust::getAdid",
                             com_adjust_sdk_Adjust_getAdid);

    translator_divert_method("Firebase.Crashlytics.dll::Firebase.Crashlytics.Crashlytics::Log",
                             Firebase_Crashlytics_Crashlytics_Log);

    translator_divert_method("Firebase.Crashlytics.dll::Firebase.Crashlytics.Crashlytics::SetUserId",
                             Firebase_Crashlytics_Crashlytics_SetUserId);

    translator_divert_method("Firebase.Analytics.dll::Firebase.Analytics.FirebaseAnalytics::.cctor",
                             Firebase_Analytics_FirebaseAnalytics_cctor);

    translator_divert_method("Firebase.Analytics.dll::Firebase.Analytics.FirebaseAnalytics::SetAnalyticsCollectionEnabled",
                             Firebase_Analytics_FirebaseAnalytics_SetAnalyticsCollectionEnabled);

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
     * We divert this method to avoid the dependency on the libFastAES.so
     * native library, it's the only method needed from it.
     */
    translator_divert_method("FastAES.dll::FastAES::NativeDecrypt", FastAES_NativeDecrypt);

    translator_divert_method("Assembly-CSharp.dll::Framework.Network.Download.AssetDownloader::IsStorageEnough",
                            Framework_Network_Download_AssetDownloader_IsStorageEnough);

    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer::InitialisePlatform",
                            RenderHeads_Media_AVProVideo_AndroidMediaPlayer_InitializePlatform);

    translator_divert_method("Assembly-CSharp.dll::Adam.Framework.Notification.Notification::SetupNotification",
                             Adam_Framework_Notification_Notification_SetupNotification);

    translator_divert_method("UnityEngine.CoreModule.dll::UnityEngine.Logger::set_logEnabled",
                             UnityEngine_Logger_set_logEnabled);

    translator_divert_method("LocalizeTime.dll::Dark.Localization.LocalizeTime::GetLocalTimeZoneId",
                             Dark_Localization_LocalizeTime_GetLocalTimeZoneId);

#ifdef _WIN32
    /*
     * To make things easier on Windows, replace the system random provider.
     * The default one uses /dev/urandom.
     */
    translator_divert_method("mscorlib.dll::System.Security.Cryptography.RNGCryptoServiceProvider::Check",
                             System_Security_Cryptography_RNGCryptoServiceProvider_Check);

    translator_divert_method("mscorlib.dll::System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes",
                             System_Security_Cryptography_RNGCryptoServiceProvider_GetBytes);
#endif

    InitializeInput();
    InitializeOcto();
/*
 * Downsizes the gRPC thread pool
 */
    translator_divert_method("Assembly-CSharp-firstpass.dll::Grpc.Core.Internal.GrpcThreadPool::.ctor",
                             Grpc_Core_Internal_GrpcThreadPool_ctor);
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
    setup->target = "192.168.4.47:8087";
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

static std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    return getPathToModule(nullptr);
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}

static int gameMain(int argc, char **argv) {
    auto executableDirectory = getExecutablePath().parent_path();

    OctoContentStorage storage(executableDirectory / "content");
    contentStorageInstance = &storage;

    GLESImplementationType gles = GLESImplementationType::Native;

#ifdef _WIN32
    bool shouldHookWGL = true;

    /*
     * Windows OpenGL ES seems to be of poor quality, so it's likely a good
     * idea to default to ANGLE there. It's certainly common across OpenGL
     * ES users to do so.
     */
    gles = GLESImplementationType::ANGLE;
#endif

    for(int index = 1; index < argc; index++) {
        if(strcmp(argv[index], "-angle") == 0) {
            gles = GLESImplementationType::ANGLE;
        } else if(strcmp(argv[index], "-native-gles") == 0) {
            gles = GLESImplementationType::Native;
        }

#ifdef _WIN32
        if(strcmp(argv[index], "-dont-hook-wgl") == 0) {
            shouldHookWGL = false;
        }
#endif

#ifndef _WIN32
        if(strcmp(argv[index], "-always-emulate-astc") == 0) {
            GLESContextShim::AlwaysEmulateASTC = true;
        } else if(strcmp(argv[index], "-never-recompress-astc") == 0) {
            GLESContextShim::NeverRecompressASTC = true;
        }
#endif
    }

#ifdef _WIN32
    if(shouldHookWGL) {
        replaceUnityWGL(gles);
    }
#else
    initializeSDLGLES(gles);
#endif

    int result = PlayerMain(argc, argv);

    contentStorageInstance = nullptr;

    return result;
}

int main(int argc, char **argv) {
    if(!applyUnityPatches())
        return 1;

    translator_set_post_initialize_callback(postInitialize);
    translator_set_grpc_redirection_callback(grpcRedirection);

    return translator_main(argc, argv, gameMain);
}

#include <filesystem>
#include <translator_api.h>

#include <cstdio>
#include <array>
#include <random>
#include <algorithm>

#include "UnityPatches.h"
#include "Il2CppUtilities.h"
#include "Input.h"
#include <ClientDataAccess/OctoContentStorage.h>
#include "Octo.h"
#include "GameEntryPoint.h"
#include "GameServerInterface.h"

#ifdef BUILDING_WITH_MPV
#include <VideoPlayer/AVProVideoNativeBypass.h>
#endif

#ifdef _WIN32
#include "WindowsHelpers.h"
#include <GLES/WGL/WGLHooking.h>
#else
#include <GLES/SDL/SDLWrapper.h>
#endif

ClientDataAccess::OctoContentStorage *contentStorageInstance;
static bool DownscalingDisabled = false;

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

#ifndef BUILDING_WITH_MPV
/*
 * When we're built without mpv, always fail video init.
 */
static bool RenderHeads_Media_AVProVideo_AndroidMediaPlayer_InitializePlatform(void *original) {
    printf("RenderHeads.Media.AVProVideo.AndroidMediaPlayer::InitializePlatform\n");

    return false;
}
#endif

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

static void System_Security_Cryptography_RNGCryptoServiceProvider_GetBytes(Il2CppObject *this_, Il2CppArray *outPtr, void *original) {
    (void)this_;
    (void)original;


    ArrayWrapper<unsigned char> out(outPtr);

    printf("System.Security.Cryptography.RNGCryptoServiceProvider::GetBytes(%p, %zu)\n", out.data(), out.size());

    using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;
    std::generate(out.data(), out.data() + out.size(), random_bytes_engine());

}
#endif

/*
 * Avoids a JNI callout to java/util/TimeZone.
 */
static Il2CppString *Dark_Localization_LocalizeTime_GetLocalTimeZoneId(Il2CppString *(*original)()) {
    return stringFromUtf8("Etc/UTC");
}

static void UniWebViewInterface_CheckPlatform(void *original) {
    (void)original;
}

static void UnityEngine_Screen_SetResolution(int32_t a, int32_t b, int32_t fullScreenMode, int32_t c,
                                            void (*original)(int32_t a, int32_t b, int32_t fullScreenMode, int32_t c)) {

    if(DownscalingDisabled) {
        printf("UnityEngine.Screen::SetResolution: suppressing resize to %dx%d, full screen mode %d, parameter 4: %d\n",
               a, b, fullScreenMode, c);

    } else {

        original(a, b, fullScreenMode, c);
    }
}

INTERPOSE_ICALL("UnityEngine.Screen::SetResolution", UnityEngine_Screen_SetResolution);

static Il2CppString *Dark_Entrypoint_Config_Api_MakeMasterDataUrl(Il2CppString *version,
                                                                  Il2CppString *(*original)(Il2CppString *version)) {

    auto url = stringToUtf8(original(version));
    if(url.ends_with(".e")) {
        url.erase(url.size() - 2);
    }

    return stringFromUtf8(url);
}

static Il2CppArray *Dark_StateMachine_HandleNet_DecryptMasterData(Il2CppObject *this_, Il2CppArray *in, void *original) {
    return in;
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

    translator_divert_method("Assembly-CSharp.dll::Dark.StateMachine.HandleNet::DecryptMasterData",
                             Dark_StateMachine_HandleNet_DecryptMasterData);

    translator_divert_method("Assembly-CSharp.dll::Dark.Entrypoint.Config/Api::MakeMasterDataUrl",
                             Dark_Entrypoint_Config_Api_MakeMasterDataUrl);

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

    translator_divert_method("Assembly-CSharp.dll::Framework.Network.Download.AssetDownloader::IsStorageEnough",
                            Framework_Network_Download_AssetDownloader_IsStorageEnough);

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

    /*
     * We report that we are *not* running on Android, so we need to disable
     * the platform check in the WebView, since we *do* provide the Android
     * JNI API to it.
     */
    translator_divert_method("Assembly-CSharp.dll::UniWebViewInterface::CheckPlatform", UniWebViewInterface_CheckPlatform);

    InitializeInput();
    InitializeOcto();
/*
 * Downsizes the gRPC thread pool
 */
    translator_divert_method("Assembly-CSharp-firstpass.dll::Grpc.Core.Internal.GrpcThreadPool::.ctor",
                             Grpc_Core_Internal_GrpcThreadPool_ctor);

#ifdef BUILDING_WITH_MPV

    installAVProVideoNativeBypass();
#else
    /*
     * If we're building without mpv, always report a failure to initialize
     * the platform media player. The .NET portion of the middleware will
     * fall back to dummy rendering without touching the Java API.
     */
    translator_divert_method("Assembly-CSharp.dll::RenderHeads.Media.AVProVideo.AndroidMediaPlayer::InitialisePlatform",
                            RenderHeads_Media_AVProVideo_AndroidMediaPlayer_InitializePlatform);
#endif
}

static std::filesystem::path getExecutablePath() {
#ifdef _WIN32
    return getPathToModule(nullptr);
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}

int gameMain(int argc, char **argv, GameInvokeUnity unityEntryPoint, void *unityInvocationPackage) {
    Il2CppVMCharacteristics::initialize();

    auto executableDirectory = getExecutablePath().parent_path();

    ClientDataAccess::OctoContentStorage storage(ClientDataAccess::OctoContentStorage::findNewestOctoList(executableDirectory / "content"));
    contentStorageInstance = &storage;

    printf("argc: %d\n", argc);

    for(int index = 1; index < argc; index++) {
        printf("arg[%d] = '%s'\n", index, argv[index]);
        if(strcmp(argv[index], "-disable-touch-emulation") == 0) {
            EmulateTouchInput = false;
        } else if(strcmp(argv[index], "-gameserver") == 0) {
            ++index;
            if(index == argc)
                break;

            setGameServerEndpoint(argv[index]);
        } else if(strcmp(argv[index], "-disable-downscale") == 0) {
            DownscalingDisabled = true;
        }
    }

    int result = unityEntryPoint(unityInvocationPackage);

    contentStorageInstance = nullptr;

    /*
     * Unity doesn't really support nicer shutdown.
     */
    exit(result);

    return result;
}

bool gameEarlyInit() {
    if(!applyUnityPatches())
        return false;

#ifdef _WIN32
    installWGLOverlayHook();
#endif

    translator_set_post_initialize_callback(postInitialize);

    return true;
}


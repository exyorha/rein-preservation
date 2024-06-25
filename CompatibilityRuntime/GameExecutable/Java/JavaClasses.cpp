#include <Java/JNIGlobalState.h>
#include <Java/JNIObject.h>
#include <Java/JNIClass.h>
#include <Java/JNIThrowable.h>
#include <Java/JNIString.h>
#include <Java/JNIAccessibleObject.h>
#include <Java/JNIField.h>
#include <Java/AndroidUtilLog.h>
#include <Java/JNIUnityPlayer.h>
#include <Java/JNIUnityReflectionHelper.h>
#include <Java/AdjustSdkAdjust.h>
#include <Java/LiliumDefaultAttestCallback.h>
#include <Java/UniWebViewInterface.h>
#include <Java/JNIMethod.h>
#include <Java/JNIConstructor.h>
#include <Java/JNIFloatArray.h>
#include <Java/JNISystem.h>
#include <Java/NativeGallery.h>
#include <Java/NativeGalleryPermissionReceiver.h>
#include <Java/AndroidContext.h>

#include <VideoPlayer/Java/AVProMobileVideo.h>
#include <VideoPlayer/Java/AVProVideoPlayer.h>

void JNIGlobalState::init() {
    registerClass<JNIObject>();
    registerClass<JNIClass>();
    registerClass<JNIThrowable>();
    registerClass<JNIString>();
    registerClass<JNIAccessibleObject>();
    registerClass<JNIField>();
    registerClass<JNIExecutable>();
    registerClass<JNIMethod>();
    registerClass<JNIConstructor>();
    registerClass<JNIFloatArray>();
    registerClass<JNISystem>();

    registerClass<AndroidUtilLog>();
    registerClass<JNIUnityPlayer>();
    registerClass<JNIUnityReflectionHelper>();
    registerClass<AdjustSdkAdjust>();

    registerClass<LiliumDefaultAttestCallback>();

    registerClass<UniWebViewInterface>();

    registerClass<AVProMobileVideo>();
    registerClass<AVProVideoPlayer>();

    registerClass<NativeGallery>();
    registerClass<NativeGalleryPermissionReceiver>();
    registerClass<AndroidContext>();

    auto context = std::make_shared<AndroidContext>();
    JNIUnityPlayer::currentActivity = context;
}

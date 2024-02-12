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

void JNIGlobalState::init() {
    registerClass<JNIObject>();
    registerClass<JNIClass>();
    registerClass<JNIThrowable>();
    registerClass<JNIString>();
    registerClass<JNIAccessibleObject>();
    registerClass<JNIField>();

    registerClass<AndroidUtilLog>();
    registerClass<JNIUnityPlayer>();
    registerClass<JNIUnityReflectionHelper>();
    registerClass<AdjustSdkAdjust>();

    registerClass<LiliumDefaultAttestCallback>();

    registerClass<UniWebViewInterface>();
}

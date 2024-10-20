#include <Java/JNIUnityPlayer.h>
#include <Java/JNIClass.h>

std::shared_ptr<JNIObject>JNIUnityPlayer::currentActivity;

std::shared_ptr<JNIClass> JNIUnityPlayer::makeClass() {
    auto co = std::make_shared<JNIClass>("com/unity3d/player/UnityPlayer", parent("java/lang/Object"));

    co->registerField("currentActivity", "Ljava/lang/Object;", &JNIUnityPlayer::currentActivity);

    return co;
}


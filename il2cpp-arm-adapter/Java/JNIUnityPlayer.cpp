#include <Java/JNIUnityPlayer.h>
#include <Java/JNIClass.h>

std::shared_ptr<JNIClass> JNIUnityPlayer::makeClass() {
    auto co = std::make_shared<JNIClass>("com/unity3d/player/UnityPlayer", parent("java/lang/Object"));
    return co;
}


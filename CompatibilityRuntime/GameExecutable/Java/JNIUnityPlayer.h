#ifndef JAVA_JNI_UNITY_PLAYER_H
#define JAVA_JNI_UNITY_PLAYER_H

#include <Java/JNIObject.h>

class JNIUnityPlayer final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();
};

#endif

#ifndef JAVA_JNI_UNITY_REFLECTION_HELPER_H
#define JAVA_JNI_UNITY_REFLECTION_HELPER_H

#include <Java/JNIObject.h>

class JNIUnityReflectionHelper final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static std::shared_ptr<JNIObject> getConstructorID(Il2CppArray *args);
    static std::shared_ptr<JNIObject> getMethodID(Il2CppArray *args);
    static std::shared_ptr<JNIObject> getFieldID(Il2CppArray *args);
    static std::shared_ptr<JNIObject> getFieldSignature(Il2CppArray *args);
    static std::shared_ptr<JNIObject> newProxyInstance(Il2CppArray *args);
    static void setNativeExceptionOnProxy(Il2CppArray *args);
};

#endif


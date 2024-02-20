#ifndef JAVA_JNI_UNITY_REFLECTION_HELPER_H
#define JAVA_JNI_UNITY_REFLECTION_HELPER_H

#include <Java/JNIObject.h>

class JNIString;
class JNIField;

class JNIUnityReflectionHelper final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static std::shared_ptr<JNIObject> getConstructorID(std::shared_ptr<JNIClass> classPtr, std::shared_ptr<JNIString> signature);
    static std::shared_ptr<JNIObject> getMethodID(std::shared_ptr<JNIClass> classPtr, std::shared_ptr<JNIString> name,
                                                  std::shared_ptr<JNIString> signature, bool isStatic);
    static std::shared_ptr<JNIObject> getFieldID(std::shared_ptr<JNIClass> classPtr, std::shared_ptr<JNIString> name,
                                                 std::shared_ptr<JNIString> signature, bool isStatic);
    static std::shared_ptr<JNIObject> getFieldSignature(std::shared_ptr<JNIField> fieldPtr);
    static std::shared_ptr<JNIObject> newProxyInstance(int64_t intParam, std::shared_ptr<JNIClass> classPtr);
    static void setNativeExceptionOnProxy(std::shared_ptr<JNIObject> objectPtr, int64_t intParam, bool boolParam);
};

#endif


#ifndef JAVA_ANDROID_UTIL_LOG_H
#define JAVA_ANDROID_UTIL_LOG_H

#include <Java/JNIObject.h>

class AndroidUtilLog final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static std::shared_ptr<JNIObject> getStackTraceString(Il2CppArray *args);
};


#endif

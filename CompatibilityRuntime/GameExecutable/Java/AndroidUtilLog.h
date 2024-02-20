#ifndef JAVA_ANDROID_UTIL_LOG_H
#define JAVA_ANDROID_UTIL_LOG_H

#include <Java/JNIObject.h>

class JNIThrowable;

class AndroidUtilLog final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static std::shared_ptr<JNIObject> getStackTraceString(std::shared_ptr<JNIThrowable> throwable);
};


#endif

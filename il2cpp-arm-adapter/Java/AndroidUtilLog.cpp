#include <Java/AndroidUtilLog.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>

std::shared_ptr<JNIClass> AndroidUtilLog::makeClass() {
    auto co = std::make_shared<JNIClass>("android/util/Log", parent("java/lang/Object"));
    co->registerMethod("getStackTraceString", "(Ljava/lang/Throwable;)Ljava/lang/String;", &AndroidUtilLog::getStackTraceString);
    return co;
}

std::shared_ptr<JNIObject> AndroidUtilLog::getStackTraceString(Il2CppArray *args) {
    printf("AndroidUtilLog::getStackTraceString\n");
    return std::make_shared<JNIString>();
}


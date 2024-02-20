#include <Java/AndroidUtilLog.h>
#include <Java/JNIClass.h>
#include <Java/JNIString.h>
#include <Java/JNIThrowable.h>
#include <Java/JNIState.h>

#include <cinttypes>

std::shared_ptr<JNIClass> AndroidUtilLog::makeClass() {
    auto co = std::make_shared<JNIClass>("android/util/Log", parent("java/lang/Object"));
    co->registerMethod("getStackTraceString", "(Ljava/lang/Throwable;)Ljava/lang/String;", &AndroidUtilLog::getStackTraceString);
    return co;
}

std::shared_ptr<JNIObject> AndroidUtilLog::getStackTraceString(std::shared_ptr<JNIThrowable> throwable) {
    printf("AndroidUtilLog::getStackTraceString(%p)\n", throwable.get());
    return std::make_shared<JNIString>();
}

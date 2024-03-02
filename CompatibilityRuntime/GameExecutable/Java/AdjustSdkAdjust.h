#ifndef JAVA_ADJUST_SDK_ADJUST_H
#define JAVA_ADJUST_SDK_ADJUST_H

#include <Java/JNIObject.h>

class AdjustSdkAdjust final : public JNIObject {
public:
    static std::shared_ptr<JNIClass> makeClass();

    static void gdprForgetMe(const std::shared_ptr<JNIObject> &arg);
};

#endif

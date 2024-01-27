#include <Java/AdjustSdkAdjust.h>
#include <Java/JNIClass.h>

std::shared_ptr<JNIClass> AdjustSdkAdjust::makeClass() {
    auto co = std::make_shared<JNIClass>("com/adjust/sdk/Adjust", parent("java/lang/Object"));
    return co;
}


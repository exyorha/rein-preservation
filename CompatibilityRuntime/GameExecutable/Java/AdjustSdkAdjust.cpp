#include <Java/AdjustSdkAdjust.h>
#include <Java/JNIClass.h>
#include <Java/JNIState.h>

std::shared_ptr<JNIClass> AdjustSdkAdjust::makeClass() {
    auto co = std::make_shared<JNIClass>("com/adjust/sdk/Adjust", parent("java/lang/Object"));
    co->registerMethod("gdprForgetMe", "(Ljava/lang/Object;)V", &gdprForgetMe);
    co->registerMethod("onResume", "()V", &onResume);

    return co;
}

void AdjustSdkAdjust::gdprForgetMe(const std::shared_ptr<JNIObject> &arg) {

}

void AdjustSdkAdjust::onResume() {

}

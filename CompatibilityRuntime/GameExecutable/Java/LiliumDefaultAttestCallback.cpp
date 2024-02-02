#include <Java/LiliumDefaultAttestCallback.h>

std::shared_ptr<JNIClass> LiliumDefaultAttestCallback::makeClass() {
    auto co = std::make_shared<JNIClass>("com/nekolaboratory/Lilium/DefaultAttestCallback", parent("java/lang/Object"));

    return co;
}

#include <Java/AndroidContext.h>
#include <Java/JNIClass.h>

AndroidContext::AndroidContext() : JNIObject(parent("android/content/Context")) {

}

AndroidContext::~AndroidContext() = default;

std::shared_ptr<JNIClass> AndroidContext::makeClass() {
    auto co = std::make_shared<JNIClass>("android/content/Context", parent("java/lang/Object"));

    return co;
}


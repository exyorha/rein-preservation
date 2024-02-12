#include <Java/UniWebViewInterface.h>

std::shared_ptr<JNIClass> UniWebViewInterface::makeClass() {
    auto co = std::make_shared<JNIClass>("com/onevcat/uniwebview/UniWebViewInterface", parent("java/lang/Object"));

    return co;
}


#include <Java/JNIAccessibleObject.h>
#include <Java/JNIClass.h>

std::shared_ptr<JNIClass> JNIAccessibleObject::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/AccessibleObject", parent("java/lang/Object"));
    return co;
}


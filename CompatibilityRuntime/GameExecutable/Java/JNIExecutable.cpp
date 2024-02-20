#include <Java/JNIExecutable.h>
#include <Java/JNIClass.h>

JNIExecutable::JNIExecutable(const std::shared_ptr<JNIClass> &objectClass) : JNIAccessibleObject(objectClass) {

}

JNIExecutable::~JNIExecutable() = default;

std::shared_ptr<JNIClass> JNIExecutable::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/Executable", parent("java/lang/reflect/AccessibleObject"));
    return co;
}



#include <Java/JNIAccessibleObject.h>
#include <Java/JNIClass.h>

JNIAccessibleObject::JNIAccessibleObject(const std::shared_ptr<JNIClass> &objectClass) : JNIObject(objectClass) {

}

JNIAccessibleObject::~JNIAccessibleObject() = default;


std::shared_ptr<JNIClass> JNIAccessibleObject::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/AccessibleObject", parent("java/lang/Object"));
    return co;
}


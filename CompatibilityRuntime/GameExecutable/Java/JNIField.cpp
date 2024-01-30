#include <Java/JNIField.h>
#include <Java/JNIClass.h>

std::shared_ptr<JNIClass> JNIField::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/Field", parent("java/lang/reflect/AccessibleObject"));
    co->registerMethod("getDeclaringClass", "()Ljava/lang/Class;", static_cast<JNIObjectMethod>(&JNIField::getDeclaringClass));
    return co;
}

std::shared_ptr<JNIObject> JNIField::getDeclaringClass(Il2CppArray *args) {
    printf("JNIField::getDeclaringClass\n");
    return {};
}

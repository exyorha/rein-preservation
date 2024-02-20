#include <Java/JNIField.h>
#include <Java/JNIClass.h>

JNIField::JNIField(const std::shared_ptr<JNIClass> &declaringClass, const JNIClass::RegisteredField &field) :
    JNIAccessibleObject(parent("java/lang/reflect/Field")),

    m_declaringClass(declaringClass), m_field(field) {

}

JNIField::~JNIField() = default;

std::shared_ptr<JNIClass> JNIField::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/Field", parent("java/lang/reflect/AccessibleObject"));
    co->registerMethod("getDeclaringClass", "()Ljava/lang/Class;", &JNIField::getDeclaringClass);
    return co;
}

std::shared_ptr<JNIObject> JNIField::getDeclaringClass() {
    return m_declaringClass;
}

#include <Java/JNIMethod.h>
#include <Java/JNIClass.h>

JNIMethod::JNIMethod(const std::shared_ptr<JNIClass> &declaringClass, const JNIClass::RegisteredMethod &method) :
    JNIExecutable(parent("java/lang/reflect/Method")),
    m_declaringClass(declaringClass), m_method(method) {

}

JNIMethod::~JNIMethod() = default;

std::shared_ptr<JNIClass> JNIMethod::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/Method", parent("java/lang/reflect/Executable"));
    return co;
}

const JNIClass::RegisteredMethod *JNIMethod::method() const {
    return &m_method;
}

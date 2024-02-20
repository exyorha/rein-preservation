#include <Java/JNIConstructor.h>
#include <Java/JNIClass.h>

JNIConstructor::JNIConstructor(const std::shared_ptr<JNIClass> &declaringClass, const JNIClass::RegisteredMethod &method) :
    JNIExecutable(parent("java/lang/reflect/Constructor")),
    m_declaringClass(declaringClass), m_method(method) {

}

JNIConstructor::~JNIConstructor() = default;

std::shared_ptr<JNIClass> JNIConstructor::makeClass() {
    std::shared_ptr<JNIClass> co = std::make_shared<JNIClass>("java/lang/reflect/Constructor", parent("java/lang/reflect/Executable"));
    return co;
}

const JNIClass::RegisteredMethod *JNIConstructor::method() const {
    return &m_method;
}


#include <Java/JNIClass.h>

#include <sstream>

JNIClass::JNIClass(const std::string_view &name, const std::shared_ptr<JNIClass> &parentClass) : m_name(name), m_parent(parentClass) {

}

JNIClass::~JNIClass() = default;

std::shared_ptr<JNIClass> JNIClass::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/Class", parent("java/lang/Object"));
    return co;
}

void JNIClass::registerMethod(const std::string_view &name, const std::string_view &signature,
                              MethodInvokable method) {

    m_methods.emplace_back(RegisteredMethod { .name = name, .signature = signature, .invokable = method });
}

const MethodInvokable *JNIClass::doGetMethodID(const std::string_view &name, const std::string_view &signature, bool isStatic) {

    for(const auto &method : m_methods) {
        if(method.name == name && method.signature == signature && isInvokableStatic(method.invokable) == isStatic) {
            return &method.invokable;
        }
    }

    if(m_parent)
        return m_parent->doGetMethodID(name, signature, isStatic);

    return nullptr;
}

const MethodInvokable * JNIClass::getMethodID(const std::string_view &name, const std::string_view &signature) {
    printf("JNI: GetMethodID: class %.*s, method '%.*s', signature '%.*s'\n",
           static_cast<int>(m_name.size()), m_name.data(),
           static_cast<int>(name.size()), name.data(),
           static_cast<int>(signature.size()), signature.data());

    auto invokable = doGetMethodID(name, signature, false);
    if(invokable)
        return invokable;

    std::stringstream error;
    error << "No method " << name << " with signature " << signature << " in " << m_name;
    throw std::runtime_error(error.str());
}

const MethodInvokable * JNIClass::getStaticMethodID(const std::string_view &name, const std::string_view &signature) {
    printf("JNI: GetStaticMethodID: class %.*s, method '%.*s', signature '%.*s'\n",
           static_cast<int>(m_name.size()), m_name.data(),
           static_cast<int>(name.size()), name.data(),
           static_cast<int>(signature.size()), signature.data());

    auto invokable = doGetMethodID(name, signature, true);
    if(invokable)
        return invokable;

    std::stringstream error;
    error << "No static method " << name << " with signature " << signature << " in " << m_name;
    throw std::runtime_error(error.str());
}

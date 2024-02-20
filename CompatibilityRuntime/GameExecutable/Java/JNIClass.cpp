#include <Java/JNIClass.h>

#include <sstream>

JNIClass::JNIClass(const std::string_view &name, const std::shared_ptr<JNIClass> &parentClass) :
    JNIObject(nullptr),
    m_name(name), m_parent(parentClass) {

}

JNIClass::~JNIClass() = default;

std::shared_ptr<JNIClass> JNIClass::makeClass() {
    auto co = std::make_shared<JNIClass>("java/lang/Class", parent("java/lang/Object"));
    return co;
}

void JNIClass::registerMethod(const std::string_view &name, const std::string_view &signature,
                              MethodInvokable &&method) {

    m_methods.emplace_back(RegisteredMethod { .name = name, .signature = signature, .invokable = std::move(method) });
}

void JNIClass::registerField(const std::string_view &name, const std::string_view &signature, bool isStatic) {
    m_fields.emplace_back(RegisteredField{
        .name = name,
        .signature = signature,
        .isStatic = isStatic
    });
}

auto JNIClass::doGetMethodID(const std::string_view &name, const std::string_view &signature, bool isStatic) -> const RegisteredMethod * {

    for(const auto &method : m_methods) {
        if(method.name == name && method.signature == signature && isInvokableStatic(method.invokable) == isStatic) {
            return &method;
        }
    }

    if(m_parent)
        return m_parent->doGetMethodID(name, signature, isStatic);

    return nullptr;
}

auto JNIClass::doGetFieldID(const std::string_view &name, const std::string_view &signature, bool isStatic) ->
    const RegisteredField * {

    for(const auto &field : m_fields) {
        if(field.name == name && field.signature == signature && field.isStatic) {
            return &field;
        }
    }

    if(m_parent)
        return m_parent->doGetFieldID(name, signature, isStatic);

    return nullptr;
}

auto JNIClass::getMethodID(const std::string_view &name, const std::string_view &signature) -> const RegisteredMethod * {
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

auto JNIClass::getStaticMethodID(const std::string_view &name, const std::string_view &signature) -> const RegisteredMethod * {
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

auto JNIClass::getFieldID(const std::string_view &name, const std::string_view &signature) -> const RegisteredField * {
    printf("JNI: GetFieldID: class %.*s, field '%.*s', signature '%.*s'\n",
           static_cast<int>(m_name.size()), m_name.data(),
           static_cast<int>(name.size()), name.data(),
           static_cast<int>(signature.size()), signature.data());

    auto invokable = doGetFieldID(name, signature, false);
    if(invokable)
        return invokable;

    std::stringstream error;
    error << "No field " << name << " with signature " << signature << " in " << m_name;
    throw std::runtime_error(error.str());
}

auto JNIClass::getStaticFieldID(const std::string_view &name, const std::string_view &signature) -> const RegisteredField * {
    printf("JNI: GetStaticFieldID: class %.*s, field '%.*s', signature '%.*s'\n",
           static_cast<int>(m_name.size()), m_name.data(),
           static_cast<int>(name.size()), name.data(),
           static_cast<int>(signature.size()), signature.data());

    auto invokable = doGetFieldID(name, signature, true);
    if(invokable)
        return invokable;

    std::stringstream error;
    error << "No static field " << name << " with signature " << signature << " in " << m_name;
    throw std::runtime_error(error.str());
}

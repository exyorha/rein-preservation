#include "ICallMethod.h"

#include <il2cpp-tabledefs.h>

#include <stdexcept>

ICallMethod::ICallMethod(std::string &&name, const MethodInfo *method) :
    m_name(std::move(name)),
    m_returnType(il2cpp_method_get_return_type(method)) {

    m_cName.reserve(m_name.size());
    bool isFirst = true;
    for(auto ch : m_name) {
        if(validCIdentifierCharacter(ch, isFirst)) {
            m_cName.push_back(ch);
        } else {
            m_cName.push_back('_');
        }

        isFirst = false;
    }

    auto argumentCount = il2cpp_method_get_param_count(method);
    m_parameterTypes.reserve(argumentCount + 1);

    uint32_t flags, iflags;
    flags = il2cpp_method_get_flags(method, &iflags);

    if(!(flags & METHOD_ATTRIBUTE_STATIC)) {
        m_parameterTypes.emplace_back(ICallType::makePointer()); // C# 'this' as the first argument
    }

    for(unsigned int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
        auto &parameter = m_parameterTypes.emplace_back(il2cpp_method_get_param(method, argumentIndex));
        if(parameter.isVoid())
            throw std::logic_error("arguments should not be 'void'");
    }
}

ICallMethod::~ICallMethod() = default;

ICallMethod::ICallMethod(ICallMethod &&other) noexcept = default;

ICallMethod &ICallMethod::operator =(ICallMethod &&other) noexcept = default;

bool ICallMethod::validCIdentifierCharacter(char ch, bool isFirst) {
    return
        (ch >= 'A' && ch <= 'Z') ||
        (ch >= 'a' && ch <= 'z') ||
        (ch >= '0' && ch <= '9' && !isFirst) ||
        ch == '_';
}


#ifndef ICALL_METHOD_H
#define ICALL_METHOD_H

#include <string>
#include <optional>
#include <vector>

#include <il2cpp-api.h>

#include "ICallType.h"

class ICallMethod {
public:
    ICallMethod(std::string &&name, const MethodInfo *method);
    ~ICallMethod();

    ICallMethod(const ICallMethod &other) = delete;
    ICallMethod &operator =(const ICallMethod &other) = delete;

    ICallMethod(ICallMethod &&other) noexcept;
    ICallMethod &operator =(ICallMethod &&other) noexcept;

    inline const std::string name() const {
        return m_name;
    }

    inline const std::string cName() const {
        return m_cName;
    }

    inline const ICallType &returnType() const {
        return m_returnType;
    }

    inline const std::vector<ICallType> &parameterTypes() const {
        return m_parameterTypes;
    }

private:
    static bool validCIdentifierCharacter(char ch, bool isFirst);

    std::string m_name;
    std::string m_cName;
    ICallType m_returnType;
    std::vector<ICallType> m_parameterTypes;
};

#endif

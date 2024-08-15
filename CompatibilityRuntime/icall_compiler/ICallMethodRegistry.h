#ifndef ICALL_METHOD_REGISTRY_H
#define ICALL_METHOD_REGISTRY_H

#include <il2cpp-api.h>

#include <string>
#include <vector>

#include "ICallMethod.h"

class ICallMethodRegistry {
public:
    ICallMethodRegistry();
    ~ICallMethodRegistry();

    ICallMethodRegistry(const ICallMethodRegistry &other) = delete;
    ICallMethodRegistry &operator =(const ICallMethodRegistry &other) = delete;

    inline const std::vector<ICallMethod> &methods() const {
        return m_methods;
    }

private:
    void addClass(Il2CppClass* klass);
    void addMethod(const std::string &className, const MethodInfo *method);

    static bool isInterestingClass(const std::string &className);

    static std::string makeClassName(Il2CppClass* klass);

    std::vector<ICallMethod> m_methods;
};

#endif

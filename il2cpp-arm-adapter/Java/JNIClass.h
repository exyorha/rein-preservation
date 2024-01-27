#ifndef JNI_CLASS_H
#define JNI_CLASS_H

#include <Java/JNIObject.h>

#include <variant>
#include <vector>

using JNIObjectMethod = std::shared_ptr<JNIObject> (JNIObject::*)(Il2CppArray *args);
using JNIStaticObjectMethod = std::shared_ptr<JNIObject> (*)(Il2CppArray *args);
using JNIStaticVoidMethod = void (*)(Il2CppArray *args);

using MethodInvokable = std::variant<JNIObjectMethod, JNIStaticObjectMethod, JNIStaticVoidMethod>;

template<typename T>
struct JNIIsInvokableStatic {

};

template<>
struct JNIIsInvokableStatic<JNIObjectMethod> {
    static constexpr bool isStatic = false;
};

template<>
struct JNIIsInvokableStatic<JNIStaticObjectMethod> {
    static constexpr bool isStatic = true;
};

template<>
struct JNIIsInvokableStatic<JNIStaticVoidMethod> {
    static constexpr bool isStatic = true;
};

static bool isInvokableStatic(const MethodInvokable &invokable) {
    return std::visit([](auto value) -> bool {
        return JNIIsInvokableStatic<decltype(value)>::isStatic;
    }, invokable);
}

class JNIClass : public JNIObject {
public:
    JNIClass(const std::string_view &name, const std::shared_ptr<JNIClass> &parentClass);
    ~JNIClass();

    static std::shared_ptr<JNIClass> makeClass();

    inline const std::string_view &name() const {
        return m_name;
    }

    const MethodInvokable *getMethodID(const std::string_view &name, const std::string_view &signature);
    const MethodInvokable *getStaticMethodID(const std::string_view &name, const std::string_view &signature);

    void registerMethod(const std::string_view &name, const std::string_view &signature,
                        MethodInvokable method);

private:
    struct RegisteredMethod {
        std::string_view name;
        std::string_view signature;
        MethodInvokable invokable;
    };

    const MethodInvokable *doGetMethodID(const std::string_view &name, const std::string_view &signature, bool isStatic);

    std::string_view m_name;
    std::shared_ptr<JNIClass> m_parent;

    std::vector<RegisteredMethod> m_methods;
};

#endif

#ifndef JNI_CLASS_H
#define JNI_CLASS_H

#include <Java/JNIObject.h>
#include <Java/JNIMethodInvocation.h>

#include <variant>
#include <vector>

class JNIClass : public JNIObject {
public:
    struct RegisteredMethod {
        std::string_view name;
        std::string_view signature;
        MethodInvokable invokable;
    };

    struct RegisteredField {
        std::string_view name;
        std::string_view signature;
        bool isStatic;
    };

    JNIClass(const std::string_view &name, const std::shared_ptr<JNIClass> &parentClass);
    ~JNIClass();

    static std::shared_ptr<JNIClass> makeClass();

    inline const std::string_view &name() const {
        return m_name;
    }

    const RegisteredMethod *getMethodID(const std::string_view &name, const std::string_view &signature);
    const RegisteredMethod *getStaticMethodID(const std::string_view &name, const std::string_view &signature);

    const RegisteredField *getFieldID(const std::string_view &name, const std::string_view &signature);
    const RegisteredField *getStaticFieldID(const std::string_view &name, const std::string_view &signature);

    void registerMethod(const std::string_view &name, const std::string_view &signature,
                        MethodInvokable &&method);

    void registerField(const std::string_view &name, const std::string_view &signature, bool isStatic);

    template<typename Result, typename Class, typename... Args>
    void registerMethod(const std::string_view &name, const std::string_view &signature, Result (Class::*meth)(Args... args)) {
        registerMethod(name, signature, std::make_unique<JNIMethodInvoker<Result, Class, Args...>>(meth));
    }

    template<typename Result, typename... Args>
    void registerMethod(const std::string_view &name, const std::string_view &signature, Result (*meth)(Args... args)) {
        registerMethod(name, signature, std::make_unique<JNIStaticMethodInvoker<Result, Args...>>(meth));
    }

    template<typename... Args>
    inline void registerConstructor(const std::string_view &signature, std::shared_ptr<JNIObject> (*meth)(Args... args)) {
        registerMethod("<init>", signature,
                       MethodInvokable(
                           std::in_place_type_t<JNIConstructorMethod>(),
                           std::make_unique<JNIConstructorMethodInvoker<Args...>>(meth)));
    }

private:
    const RegisteredMethod *doGetMethodID(const std::string_view &name, const std::string_view &signature, bool isStatic);

    const RegisteredField *doGetFieldID(const std::string_view &name, const std::string_view &signature, bool isStatic);

    std::string_view m_name;
    std::shared_ptr<JNIClass> m_parent;

    std::vector<RegisteredMethod> m_methods;
    std::vector<RegisteredField> m_fields;
};

#endif

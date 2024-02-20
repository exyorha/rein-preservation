#ifndef JAVA_JNI_METHOD_INVOCATION_H
#define JAVA_JNI_METHOD_INVOCATION_H

#include <Java/JValue.h>

#include <stdexcept>
#include <memory>
#include <variant>

class JNIObject;

template<int ArgumentsLeftToUnpack, typename Result, typename TargetClass, typename PointerType>
struct JNIInvocationArgumentUnpacker {
    template<typename... AlreadyUnpackedArgs>
    static inline Result call(PointerType callee, JNIObject &object, const JValueArray &bundle, AlreadyUnpackedArgs... args);
};

template<typename Result, typename TargetClass, typename PointerType>
struct JNIInvocationArgumentUnpacker<0, Result, TargetClass, PointerType> {
    template<typename... AlreadyUnpackedArgs>
    static inline Result call(PointerType callee, JNIObject &object, const JValueArray &bundle, AlreadyUnpackedArgs... args) {
        return (static_cast<TargetClass &>(object).*callee)(args...);
    }
};

template<int ArgumentsLeftToUnpack, typename Result, typename PointerType>
struct JNIInvocationStaticArgumentUnpacker {
    template<typename... AlreadyUnpackedArgs>
    static inline Result call(PointerType callee, const JValueArray &bundle, AlreadyUnpackedArgs... args);
};

template<typename Result, typename PointerType>
struct JNIInvocationStaticArgumentUnpacker<0, Result, PointerType> {
    template<typename... AlreadyUnpackedArgs>
    static inline Result call(PointerType callee, const JValueArray &bundle, AlreadyUnpackedArgs... args) {
        return callee(args...);
    }
};

template<int Position>
struct JNIInvocationArgumentTranslator {
    inline JNIInvocationArgumentTranslator(const JValueArray &args) : m_args(args) {

    }

    template<typename T>
    inline operator T() const {
        return static_cast<T>(m_args[Position]);
    }

private:
    const JValueArray &m_args;
};

template<int ArgumentsLeftToUnpack, typename Result, typename TargetClass, typename PointerType> template<typename... AlreadyUnpackedArgs>
inline Result JNIInvocationArgumentUnpacker<ArgumentsLeftToUnpack, Result, TargetClass, PointerType>::call(
    PointerType callee, JNIObject &object, const JValueArray &bundle, AlreadyUnpackedArgs... args
) {
    return JNIInvocationArgumentUnpacker<ArgumentsLeftToUnpack - 1, Result, TargetClass, PointerType>::call(callee, object, bundle,
        JNIInvocationArgumentTranslator<ArgumentsLeftToUnpack - 1>(bundle), args...);
}

template<int ArgumentsLeftToUnpack, typename Result, typename PointerType> template<typename... AlreadyUnpackedArgs>
inline Result JNIInvocationStaticArgumentUnpacker<ArgumentsLeftToUnpack, Result, PointerType>::call(
    PointerType callee, const JValueArray &bundle, AlreadyUnpackedArgs... args
) {
    return JNIInvocationStaticArgumentUnpacker<ArgumentsLeftToUnpack - 1, Result, PointerType>::call(callee, bundle,
        JNIInvocationArgumentTranslator<ArgumentsLeftToUnpack - 1>(bundle), args...);
}

class ArgumentUnpackingJNIInvoker {
protected:
    template<typename Class, typename Result,  typename... Args>
    inline Result unpackAndApply(Result (Class::*method)(Args... args), JNIObject &object, const JValueArray &args) {
        return JNIInvocationArgumentUnpacker<sizeof...(Args), Result, Class, decltype(method)>::call(method, object, args);
    }

    template<typename Result, typename... Args>
    inline Result unpackAndApply(Result (*method)(Args... args), const JValueArray &args) {
        return JNIInvocationStaticArgumentUnpacker<sizeof...(Args), Result, decltype(method)>::call(method, args);
    }
};

template<typename ReturnType>
class BaseJNIMethodInvoker {
protected:
    BaseJNIMethodInvoker() = default;

public:
    static constexpr bool isStatic = false;

    virtual ~BaseJNIMethodInvoker() = default;

    BaseJNIMethodInvoker(const BaseJNIMethodInvoker &other) = delete;
    BaseJNIMethodInvoker &operator =(const BaseJNIMethodInvoker &other) = delete;

    virtual ReturnType invoke(JNIObject &onObject, const JValueArray &args) = 0;
};

template<typename ReturnType>
class BaseJNIStaticMethodInvoker {
protected:
    BaseJNIStaticMethodInvoker() = default;

public:
    static constexpr bool isStatic = true;

    virtual ~BaseJNIStaticMethodInvoker() = default;

    BaseJNIStaticMethodInvoker(const BaseJNIStaticMethodInvoker &other) = delete;
    BaseJNIStaticMethodInvoker &operator =(const BaseJNIStaticMethodInvoker &other) = delete;

    virtual ReturnType invoke(const JValueArray &args) = 0;
};

class BaseJNIConstructorMethodInvoker : public BaseJNIStaticMethodInvoker<std::shared_ptr<JNIObject>> {
public:
    static constexpr bool isStatic = false;
};

template<typename ReturnType, typename Class, typename... Args>
class JNIMethodInvoker final : public BaseJNIMethodInvoker<ReturnType>, protected ArgumentUnpackingJNIInvoker {
public:
    using Pointer = ReturnType (Class::*)(Args... args);

    inline JNIMethodInvoker(Pointer pointer) : m_pointer(pointer) {

    }

    ~JNIMethodInvoker() override = default;

    ReturnType invoke(JNIObject &onObject, const JValueArray &args) override {
        if(args.size() != sizeof...(Args))
            throw std::logic_error("JNIMethodInvoker: unexpected number of arguments");

        return unpackAndApply(m_pointer, onObject, args);
    }
private:
    Pointer m_pointer;
};

template<typename ReturnType, typename... Args>
class JNIStaticMethodInvoker final : public BaseJNIStaticMethodInvoker<ReturnType>, protected ArgumentUnpackingJNIInvoker {
public:
    using Pointer = ReturnType (*)(Args... args);

    inline JNIStaticMethodInvoker(Pointer pointer) : m_pointer(pointer) {

    }

    ~JNIStaticMethodInvoker() override = default;

    ReturnType invoke(const JValueArray &args) override {

        if(args.size() != sizeof...(Args))
            throw std::logic_error("JNIMethodInvoker: unexpected number of arguments");

        return unpackAndApply(m_pointer, args);
    }

private:
    Pointer m_pointer;
};

template<typename... Args>
class JNIConstructorMethodInvoker final : public BaseJNIConstructorMethodInvoker, protected ArgumentUnpackingJNIInvoker {
public:
    using Pointer = std::shared_ptr<JNIObject> (*)(Args... args);

    inline JNIConstructorMethodInvoker(Pointer pointer) : m_pointer(pointer) {

    }

    ~JNIConstructorMethodInvoker() override = default;

    std::shared_ptr<JNIObject> invoke(const JValueArray &args) override {

        if(args.size() != sizeof...(Args))
            throw std::logic_error("JNIMethodInvoker: unexpected number of arguments");

        return unpackAndApply(m_pointer, args);
    }

private:
    Pointer m_pointer;
};

using JNIObjectMethod = std::unique_ptr<BaseJNIMethodInvoker<std::shared_ptr<JNIObject>>>;
using JNIVoidMethod = std::unique_ptr<BaseJNIMethodInvoker<void>>;
using JNIFloatMethod = std::unique_ptr<BaseJNIMethodInvoker<float>>;
using JNIIntMethod = std::unique_ptr<BaseJNIMethodInvoker<int32_t>>;
using JNILongMethod = std::unique_ptr<BaseJNIMethodInvoker<int64_t>>;
using JNIBooleanMethod = std::unique_ptr<BaseJNIMethodInvoker<bool>>;

using JNIStaticObjectMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<std::shared_ptr<JNIObject>>>;
using JNIStaticVoidMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<void>>;
using JNIStaticFloatMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<float>>;
using JNIStaticIntMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<int32_t>>;
using JNIStaticLongMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<int64_t>>;
using JNIStaticBooleanMethod = std::unique_ptr<BaseJNIStaticMethodInvoker<bool>>;
using JNIConstructorMethod = std::unique_ptr<BaseJNIConstructorMethodInvoker>;

using MethodInvokable = std::variant<
    JNIObjectMethod,
    JNIVoidMethod,
    JNIFloatMethod,
    JNIIntMethod,
    JNILongMethod,
    JNIBooleanMethod,

    JNIStaticObjectMethod,
    JNIStaticVoidMethod,
    JNIStaticFloatMethod,
    JNIStaticIntMethod,
    JNIStaticLongMethod,
    JNIStaticBooleanMethod,

    JNIConstructorMethod>;

static bool isInvokableStatic(const MethodInvokable &invokable) {
    return std::visit([](const auto &ptr) -> bool {
        return std::remove_reference<decltype(*ptr)>::type::isStatic;
    }, invokable);
}

#endif


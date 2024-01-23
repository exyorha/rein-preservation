#ifndef THUNKING_H
#define THUNKING_H

#include "support.h"
#include "JIT.h"
#include "JITThreadContext.h"
#include "ThunkManager.h"
#include <cstddef>
#include <type_traits>

void runARMCall(JITThreadContext &context);

template<typename ReturnType, typename... Args>
ReturnType __attribute__((noinline)) armcall(ReturnType (*armFunctionPointer)(Args... args), Args... args) {

    auto &context = JITThreadContext::get();

    context.pc = reinterpret_cast<uintptr_t>(armFunctionPointer);

    runARMCall(context);

    panic("rest of armcall not implemented\n");
}

template<typename ReturnType, typename... Args>
[[noreturn]] void __attribute__((noinline)) armcall_noreturn(ReturnType (*armFunctionPointer)(Args... args), Args... args) {
    armcall(armFunctionPointer, args...);

    panic("armcall returned in an armcall_noreturn function");
}

uintptr_t retrieveX86CallPointerSizedArgument(int position);
void storeX86CallPointerSizedResult(uintptr_t result);

template<typename T>
static inline auto retrieveX86CallPointer(int position) -> typename std::enable_if<!std::is_function_v<T>, T *>::type {
    return reinterpret_cast<T *>(retrieveX86CallPointerSizedArgument(position));
}

template<typename T>
auto retrieveX86CallPointer(int position) -> typename std::enable_if<std::is_function_v<T>, T *>::type {
    panic("function pointer retrieval is not implemented yet");
}

template<typename T>
static inline auto retrieveX86CallArgument(int position) -> typename std::enable_if<std::is_pointer_v<T>, T>::type {
    return retrieveX86CallPointer<typename std::remove_pointer<T>::type>(position);
}

template<typename T>
static inline auto retrieveX86CallArgument(int position) -> typename std::enable_if<std::is_integral_v<T>, T>::type {
    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot retrieve arguments larger than a pointer");

    return static_cast<T>(retrieveX86CallPointerSizedArgument(position));
}

template<typename T>
auto retrieveX86CallArgument(int position) -> typename std::enable_if<std::is_floating_point_v<T>, T>::type {
    panic("floating point argument retrieval is not implemented yet");
}

template<typename T>
static inline auto storeX86CallPointerResult(T *result) -> typename std::enable_if<!std::is_function_v<T>>::type {

    storeX86CallPointerSizedResult(reinterpret_cast<uintptr_t>(result));
}

template<typename T>
auto storeX86CallPointerResult(T *result) -> typename std::enable_if<std::is_function_v<T>>::type {

    panic("function pointer result storage is not implemented yet");
}

template<typename T>
static inline auto storeX86CallResult(T result) -> typename std::enable_if<std::is_pointer_v<T>>::type {

    storeX86CallPointerResult<typename std::remove_pointer<T>::type>(result);
}

template<bool isSigned>
struct X86CallWideningType { };

template<>
struct X86CallWideningType<false> { using Type = uintptr_t; };

template<>
struct X86CallWideningType<true> { using Type = intptr_t; };

template<typename T>
static inline auto storeX86CallResult(T result) -> typename std::enable_if<std::is_integral_v<T>>::type {
    using WideningType = typename X86CallWideningType<std::is_signed_v<T>>::Type;

    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot store results larger than a pointer");

    storeX86CallPointerSizedResult(static_cast<uintptr_t>(static_cast<WideningType>(result)));
}

template<typename T>
auto storeX86CallResult(T result) -> typename std::enable_if<std::is_floating_point_v<T>>::type {
    panic("floating point result storing is not implemented yet");
}


template<int Position>
struct X86CallTranslateArgument {
    template<typename T>
    inline operator T() const {
        return retrieveX86CallArgument<T>(Position);
    }
};

template<typename ReturnType, typename FunctionPointer, typename... Args>
struct X86CallAndTranslateResult {

    static inline void call(FunctionPointer callee, Args... args) {
        storeX86CallResult<ReturnType>(callee(args...));
    }
};

template<typename FunctionPointer, typename... Args>
struct X86CallAndTranslateResult<void, FunctionPointer, Args...> {

    static inline void call(FunctionPointer callee, Args... args) {
        callee(args...);
    }
};

template<int ArgumentsLeftToTranslate, typename X86FunctionPointer>
struct X86CallTranslatingFunctionApply {
    template<typename... AlreadyTranslatedArguments>
    static inline void apply(X86FunctionPointer callee, AlreadyTranslatedArguments... args) {
        X86CallTranslatingFunctionApply<ArgumentsLeftToTranslate - 1, X86FunctionPointer>::apply(
            callee,
            X86CallTranslateArgument<ArgumentsLeftToTranslate - 1>(),
            args...);
    }
};

template<typename X86FunctionPointer>
struct X86CallTranslatingFunctionApply<0, X86FunctionPointer> {
    template<typename... AlreadyTranslatedArguments>
    static inline void apply(X86FunctionPointer callee, AlreadyTranslatedArguments... args) {
        using ReturnType = std::invoke_result_t<X86FunctionPointer, AlreadyTranslatedArguments...>;

        X86CallAndTranslateResult<ReturnType, X86FunctionPointer, AlreadyTranslatedArguments...>::call(callee, args...);
    }
};

template<typename ReturnType, typename... Args>
void x86call(ReturnType (*x86FunctionPointer)(Args... args)) {

    X86CallTranslatingFunctionApply<sizeof...(Args), decltype(x86FunctionPointer)>::apply(x86FunctionPointer);
}

#endif



#ifndef THUNKING_H
#define THUNKING_H

#include "support.h"
#include "JIT.h"
#include "JITThreadContext.h"
#include "ThunkManager.h"
#include <cinttypes>

#include <cstddef>
#include <type_traits>

template<bool isSigned>
struct WideningType { };

template<>
struct WideningType<false> { using Type = uintptr_t; };

template<>
struct WideningType<true> { using Type = intptr_t; };

void storeARMCallPointerSizedArgument(int index, uintptr_t argument);
uintptr_t fetchARMCallPointerSizedResult();

void runARMCall(JITThreadContext &context);

extern "C" {
    intptr_t getOffsetOfThunkUtilitySlot();
    extern thread_local void *thunkUtilitySlot;
}


template<typename T>
static inline auto fetchARMCallResult() -> typename std::enable_if<std::is_integral_v<T>, T>::type {
    return static_cast<T>(fetchARMCallPointerSizedResult());
}

template<typename T>
static inline auto fetchARMCallPointerResult() -> typename std::enable_if<!std::is_function_v<T>, T *>::type {
    return reinterpret_cast<T *>(fetchARMCallPointerSizedResult());
}

template<typename T>
auto fetchARMCallPointerResult() -> typename std::enable_if<std::is_function_v<T>, T *>::type {
    panic("function pointer result thunking is not implemented yet");
}

template<typename T>
static inline auto fetchARMCallResult() -> typename std::enable_if<std::is_pointer_v<T>, T>::type {
    return fetchARMCallPointerResult<typename std::remove_pointer<T>::type>();
}

template<typename T>
struct ARMCallResultFetcher {
    static inline T fetchAndReturn() {
        return fetchARMCallResult<T>();
    }
};

template<>
struct ARMCallResultFetcher<void> {
    static inline void fetchAndReturn() { }
};

template<typename T>
static inline auto armCallStoreArgument(int argumentIndex, T arg) -> typename std::enable_if<std::is_integral_v<T>>::type {
    using WideningType = typename WideningType<std::is_signed_v<T>>::Type;

    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot store arguments larger than a pointer");

    storeARMCallPointerSizedArgument(argumentIndex, static_cast<uintptr_t>(static_cast<WideningType>(arg)));
}

template<typename T>
static inline auto armCallStorePointerArgument(int argumentIndex, T *arg) -> typename std::enable_if<!std::is_function_v<T>>::type {
    storeARMCallPointerSizedArgument(argumentIndex, reinterpret_cast<uintptr_t>(arg));
}

template<typename T>
auto armCallStorePointerArgument(int argumentIndex, T *arg) -> typename std::enable_if<std::is_function_v<T>>::type {
    panic("thunking is not yet implemented for pointer to functions passed as arguments\n");
}

template<typename T>
static inline auto armCallStoreArgument(int argumentIndex, T arg) -> typename std::enable_if<std::is_pointer_v<T>>::type {
    armCallStorePointerArgument(argumentIndex, arg);
}

/*
 * C++ stuff
 */
#if 0
template<typename T>
static inline auto armCallStoreStoreArgument(int argumentIndex, T arg) -> typename std::enable_if<std::is_reference_v<T>>::type {
    armCallStoreArgument(argumentIndex, &arg);
}
#endif
template<typename T>
static inline auto armCallStoreArgument(int argumentIndex, T arg) -> typename std::enable_if<std::is_enum_v<T>>::type {
    armCallStoreArgument(argumentIndex, static_cast<typename std::underlying_type<T>::type>(arg));
}

static inline void armCallStoreArguments(int argumentIndex) {
    (void)argumentIndex;
}

template<typename Arg, typename...Rest>
static inline void armCallStoreArguments(int argumentIndex, Arg arg, Rest... rest) {
    armCallStoreArgument(argumentIndex, arg);

    armCallStoreArguments(argumentIndex + 1, rest...);
}

template<typename ReturnType, typename... Args>
ReturnType __attribute__((noinline)) armcall(ReturnType (*armFunctionPointer)(Args... args), Args... args) {

    auto &context = JITThreadContext::get();

    context.push(context.lr());

    armCallStoreArguments(0, args...);
    printf("@@@ IMPLEMENT ARMCALL ARGUMENT SETTING\n");

    // TODO: arguments!

    context.lr() = context.pc;
    context.pc = reinterpret_cast<uintptr_t>(armFunctionPointer);

    /*
     * runARMCall will change LR.
     */
    runARMCall(context);

    context.lr() = context.pop();

    return ARMCallResultFetcher<ReturnType>::fetchAndReturn();
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

template<typename ResultType, typename... Args>
ResultType x86CallThunk(Args... args) {
    auto armFunction = reinterpret_cast<ResultType (*)(Args...)>(thunkUtilitySlot);

    return armcall(armFunction, args...);
}

template<typename T>
static auto retrieveX86CallPointer(int position) -> typename std::enable_if<std::is_function_v<T>, T *>::type {

    auto ptr = reinterpret_cast<void *>(retrieveX86CallPointerSizedArgument(position));
    if(!ptr)
        return nullptr;

    /*
     * TODO: un-thunk back to x86 if ptr is a pointer to a thunked ARM function.
     */

    auto thunk = ThunkManager::allocateX86ToARMThunkCall(ptr, reinterpret_cast<ThunkManager::X86ThunkTarget>(static_cast<T *>(x86CallThunk)));

    printf("ARM function %p thunked via %p\n", ptr, thunk);

    return reinterpret_cast<T *>(thunk);
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
    panic("floating point argument retrieval is not implemented yet\n");
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

template<typename T>
static inline auto storeX86CallResult(T result) -> typename std::enable_if<std::is_integral_v<T>>::type {
    using WideningType = typename WideningType<std::is_signed_v<T>>::Type;

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



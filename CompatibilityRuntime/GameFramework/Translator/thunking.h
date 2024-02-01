#ifndef THUNKING_H
#define THUNKING_H

#include "support.h"
#include "GlobalContext.h"
#include <Translator/JIT.h>
#include <Translator/JITThreadContext.h>
#include <Translator/ThunkManager.h>
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

void fetchX86CallFloatingPointArgument(int index, float &out);
void fetchX86CallFloatingPointArgument(int index, double &out);
void fetchX86CallFloatingPointArgument(int index, long double &out);

void storeX86CallFloatResult(float result);
void storeX86CallFloatResult(double result);
void storeX86CallFloatResult(long double result);
void storeX86CallStructureResult(const void *data, size_t size);

void runARMCall(JITThreadContext &context);

extern "C" {
    intptr_t getOffsetOfThunkUtilitySlot();
    extern thread_local void *thunkUtilitySlot;
}


template<typename ReturnType, typename... Args>
static constexpr inline ThunkManager::X86ThunkTarget createTypedX86Thunk(ReturnType (*)(Args...));

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
    if(!arg) {
        storeARMCallPointerSizedArgument(argumentIndex, 0);
        return;
    }

    /*
     * Is this an x86 to ARM thunk?
     */
    auto thunkedARMFunction = GlobalContext::get().thunkManager().lookupX86ToARMThunkCall(reinterpret_cast<ThunkManager::X86ThunkTarget>(arg));
    if(thunkedARMFunction) {
        /*
         * Yes; don't double-thunk, just return the ARM function.
         */
        storeARMCallPointerSizedArgument(argumentIndex, reinterpret_cast<uintptr_t>(thunkedARMFunction));
        return;
    }

    auto thunk =
        GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(arg), createTypedX86Thunk(arg));

    storeARMCallPointerSizedArgument(argumentIndex, reinterpret_cast<uintptr_t>(thunk));
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

    auto savedPC = context.pc;
    auto savedLR = context.lr();
   // context.push(context.pc);
   // context.push(context.lr());

    armCallStoreArguments(0, args...);

    context.pc = reinterpret_cast<uintptr_t>(armFunctionPointer);

    /*
     * runARMCall will change LR.
     */
    runARMCall(context);

    //context.lr() = context.pop();
    //context.pc = context.pop();
    context.lr() = savedLR;
    context.pc = savedPC;

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

    void *thunkedX86 = GlobalContext::get().thunkManager().lookupARMToX86ThunkCall(ptr);
    if(thunkedX86) {
        /*
         * This is a thunked x86 function pointer; pass it directly, don't double-thunk.
         */

        return reinterpret_cast<T *>(thunkedX86);
    }

    auto thunk = GlobalContext::get().thunkManager().allocateX86ToARMThunkCall(
        ptr, reinterpret_cast<ThunkManager::X86ThunkTarget>(static_cast<T *>(x86CallThunk)));

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
static inline auto retrieveX86CallArgument(int position) -> typename std::enable_if<std::is_floating_point_v<T>, T>::type {
    T result;
    fetchX86CallFloatingPointArgument(position, result);
    return result;
}

template<typename T>
static inline auto retrieveX86CallArgument(int position) -> typename std::enable_if<std::is_enum_v<T>, T>::type {
    return static_cast<T>(retrieveX86CallArgument<typename std::underlying_type<T>::type>(position));
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
    storeX86CallFloatResult(result);
}

template<typename T>
static inline auto storeX86CallResult(T result) -> typename std::enable_if<std::is_enum_v<T>>::type {
    storeX86CallResult(static_cast<typename std::underlying_type<T>::type>(result));
}

template<typename T>
inline auto storeX86CallResult(T result) -> typename std::enable_if<std::is_class_v<T> && std::is_standard_layout_v<T> && sizeof(T) <= 64>::type {
    storeX86CallStructureResult(&result, sizeof(result));
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

template<typename ReturnType, typename... Args>
void x86call() {
    return x86call(reinterpret_cast<ReturnType (*)(Args...)>(thunkUtilitySlot));
}

template<typename ReturnType, typename... Args>
static constexpr inline ThunkManager::X86ThunkTarget createTypedX86Thunk(ReturnType (*)(Args...)) {
    return static_cast<ThunkManager::X86ThunkTarget>(&x86call<ReturnType, Args...>);
}

#endif



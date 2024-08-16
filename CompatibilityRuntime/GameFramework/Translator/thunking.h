#ifndef THUNKING_H
#define THUNKING_H

#include "support.h"
#include "GlobalContext.h"
#include <Translator/JIT.h>
#include <Translator/JITThreadContext.h>
#include <Translator/ThunkManager.h>
#include <Translator/WrappedARMException.h>
#include <Translator/ARMCallCleanableFrame.h>

#include <cinttypes>

#include <cstddef>
#include <type_traits>

template<bool isSigned>
struct WideningType { };

template<>
struct WideningType<false> { using Type = uintptr_t; };

template<>
struct WideningType<true> { using Type = intptr_t; };

void storeARMCallPointerSizedArgument(JITThreadContext &context, int index, uintptr_t argument);
uintptr_t fetchARMCallPointerSizedResult(JITThreadContext &context);

void fetchX86CallFloatingPointArgument(JITThreadContext &context, int index, float &out);
void fetchX86CallFloatingPointArgument(JITThreadContext &context, int index, double &out);
void fetchX86CallFloatingPointArgument(JITThreadContext &context, int index, long double &out);

void storeX86CallFloatResult(JITThreadContext &context, float result);
void storeX86CallFloatResult(JITThreadContext &context, double result);
void storeX86CallFloatResult(JITThreadContext &context, long double result);
void storeX86CallStructureResult(JITThreadContext &context, const void *data, size_t size);

uintptr_t retrieveX86CallPointerSizedArgument(JITThreadContext &context, int position);
void storeX86CallPointerSizedResult(JITThreadContext &context, uintptr_t result);

void runARMCall(JITThreadContext &context);

void rethrowWrappedARMExceptionFromX86Call(const WrappedARMException &exception);


extern "C" {
    intptr_t getOffsetOfThunkUtilitySlot();
#if defined(_WIN32)
    void *readThunkUtilitySlot();
    void writeThunkUtilitySlot(void *value);
#else
    extern thread_local void *thunkUtilitySlot;

    static inline void *readThunkUtilitySlot() {
        return thunkUtilitySlot;
    }

    static inline void writeThunkUtilitySlot(void *value) {
        thunkUtilitySlot = value;
    }

#endif
}


template<typename ReturnType, typename... Args>
static constexpr inline ThunkManager::X86ThunkTarget createTypedX86Thunk(ReturnType (*)(Args...));

template<typename T>
[[gnu::artificial]]
static inline auto fetchARMCallResult(JITThreadContext &context) -> typename std::enable_if<std::is_integral_v<T>, T>::type {
    return static_cast<T>(fetchARMCallPointerSizedResult(context));
}

template<typename T>
[[gnu::artificial]]
static inline auto fetchARMCallPointerResult(JITThreadContext &context) -> typename std::enable_if<!std::is_function_v<T>, T *>::type {
    return reinterpret_cast<T *>(fetchARMCallPointerSizedResult(context));
}

template<typename T>
[[gnu::artificial]]
static inline auto fetchARMCallResult(JITThreadContext &context) -> typename std::enable_if<std::is_pointer_v<T>, T>::type {
    return fetchARMCallPointerResult<typename std::remove_pointer<T>::type>(context);
}

template<typename T>
struct ARMCallResultFetcher {
    [[gnu::artificial]]
    static inline T fetchAndReturn(JITThreadContext &context) {
        return fetchARMCallResult<T>(context);
    }
};

template<>
struct ARMCallResultFetcher<void> {
    [[gnu::artificial]]
    static inline void fetchAndReturn(JITThreadContext &context) { }
};

template<typename T>
[[gnu::artificial]]
static inline auto armCallStoreArgument(JITThreadContext &context, int argumentIndex, T arg) -> typename std::enable_if<std::is_integral_v<T>>::type {
    using WideningType = typename WideningType<std::is_signed_v<T>>::Type;

    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot store arguments larger than a pointer");

    storeARMCallPointerSizedArgument(context, argumentIndex, static_cast<uintptr_t>(static_cast<WideningType>(arg)));
}

template<typename T>
[[gnu::artificial]]
static inline auto armCallStorePointerArgument(JITThreadContext &context, int argumentIndex, T *arg) -> typename std::enable_if<!std::is_function_v<T>>::type {
    storeARMCallPointerSizedArgument(context, argumentIndex, reinterpret_cast<uintptr_t>(arg));
}

template<typename T>
auto armCallStorePointerArgument(JITThreadContext &context, int argumentIndex, T *arg) -> typename std::enable_if<std::is_function_v<T>>::type {
    if(!arg) {
        storeARMCallPointerSizedArgument(context, argumentIndex, 0);
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
        storeARMCallPointerSizedArgument(context, argumentIndex, reinterpret_cast<uintptr_t>(thunkedARMFunction));
        return;
    }

    auto thunk =
        GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(arg), createTypedX86Thunk(arg));

    storeARMCallPointerSizedArgument(context, argumentIndex, reinterpret_cast<uintptr_t>(thunk));
}

template<typename T>
[[gnu::artificial]]
static inline auto armCallStoreArgument(JITThreadContext &context, int argumentIndex, T arg) -> typename std::enable_if<std::is_pointer_v<T>>::type {
    armCallStorePointerArgument(context, argumentIndex, arg);
}

template<typename T>
[[gnu::artificial]]
static inline auto armCallStoreArgument(JITThreadContext &context, int argumentIndex, T arg) -> typename std::enable_if<std::is_enum_v<T>>::type {
    armCallStoreArgument(context, argumentIndex, static_cast<typename std::underlying_type<T>::type>(arg));
}

[[gnu::artificial]]
static inline void armCallStoreArguments(JITThreadContext &context, int argumentIndex) {
    (void)argumentIndex;
}

template<typename Arg, typename...Rest>
[[gnu::artificial]]
static inline void armCallStoreArguments(JITThreadContext &context, int argumentIndex, Arg arg, Rest... rest) {
    armCallStoreArgument(context, argumentIndex, arg);

    armCallStoreArguments(context, argumentIndex + 1, rest...);
}

template<typename ReturnType, typename... Args>
ReturnType armcall(ReturnType (*armFunctionPointer)(Args... args), Args... args) {

    auto &context = JITThreadContext::get();

    {
        ARMCallCleanableFrame armFrame(context);

        armCallStoreArguments(context, 0, args...);

        context.pc = reinterpret_cast<uintptr_t>(armFunctionPointer);

        /*
        * runARMCall will change LR to __compatibility_runtime_armcall_catcher,
        * containing a SVC that will return from runARMCall.
        */
        runARMCall(context);
    }

    return ARMCallResultFetcher<ReturnType>::fetchAndReturn(context);
}

template<typename ReturnType, typename... Args>
[[noreturn]] void armcall_noreturn(ReturnType (*armFunctionPointer)(Args... args), Args... args) {
    armcall(armFunctionPointer, args...);

    panic("armcall returned in an armcall_noreturn function");
}

template<typename T>
[[gnu::artificial]]
static inline auto retrieveX86CallPointer(JITThreadContext &context, int position) -> typename std::enable_if<!std::is_function_v<T>, T *>::type {
    return reinterpret_cast<T *>(retrieveX86CallPointerSizedArgument(context, position));
}

template<typename ResultType, typename... Args>
ResultType x86CallThunk(Args... args) {
    auto armFunction = reinterpret_cast<ResultType (*)(Args...)>(readThunkUtilitySlot());

    return armcall(armFunction, args...);
}

template<typename T>
static auto retrieveX86CallPointer(JITThreadContext &context, int position) -> typename std::enable_if<std::is_function_v<T>, T *>::type {

    auto ptr = reinterpret_cast<void *>(retrieveX86CallPointerSizedArgument(context, position));
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
[[gnu::artificial]]
static inline auto retrieveX86CallArgument(JITThreadContext &context, int position) -> typename std::enable_if<std::is_pointer_v<T>, T>::type {
    return retrieveX86CallPointer<typename std::remove_pointer<T>::type>(context, position);
}

template<typename T>
[[gnu::artificial]]
static inline auto retrieveX86CallArgument(JITThreadContext &context, int position) -> typename std::enable_if<std::is_integral_v<T>, T>::type {
    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot retrieve arguments larger than a pointer");

    return static_cast<T>(retrieveX86CallPointerSizedArgument(context, position));
}

template<typename T>
static inline auto retrieveX86CallArgument(JITThreadContext &context, int position) -> typename std::enable_if<std::is_floating_point_v<T>, T>::type {
    T result;
    fetchX86CallFloatingPointArgument(context, position, result);
    return result;
}

template<typename T>
[[gnu::artificial]]
static inline auto retrieveX86CallArgument(JITThreadContext &context, int position) -> typename std::enable_if<std::is_enum_v<T>, T>::type {
    return static_cast<T>(retrieveX86CallArgument<typename std::underlying_type<T>::type>(context, position));
}

template<typename T>
[[gnu::artificial]]
static inline auto storeX86CallPointerResult(JITThreadContext &context, T *result) -> typename std::enable_if<!std::is_function_v<T>>::type {

    storeX86CallPointerSizedResult(context, reinterpret_cast<uintptr_t>(result));
}

template<typename T>
auto storeX86CallPointerResult(JITThreadContext &context, T *result) -> typename std::enable_if<std::is_function_v<T>>::type {

    panic("function pointer result storage is not implemented yet");
}

template<typename T>
[[gnu::artificial]]
static inline auto storeX86CallResult(JITThreadContext &context, T result) -> typename std::enable_if<std::is_pointer_v<T>>::type {

    storeX86CallPointerResult<typename std::remove_pointer<T>::type>(context, result);
}

template<typename T>
[[gnu::artificial]]
static inline auto storeX86CallResult(JITThreadContext &context, T result) -> typename std::enable_if<std::is_integral_v<T>>::type {
    using WideningType = typename WideningType<std::is_signed_v<T>>::Type;

    static_assert(sizeof(T) <= sizeof(uintptr_t), "cannot store results larger than a pointer");

    storeX86CallPointerSizedResult(context, static_cast<uintptr_t>(static_cast<WideningType>(result)));
}

template<typename T>
[[gnu::artificial]]
inline auto storeX86CallResult(JITThreadContext &context, T result) -> typename std::enable_if<std::is_floating_point_v<T>>::type {
    storeX86CallFloatResult(context, result);
}

template<typename T>
[[gnu::artificial]]
static inline auto storeX86CallResult(JITThreadContext &context, T result) -> typename std::enable_if<std::is_enum_v<T>>::type {
    storeX86CallResult(context, static_cast<typename std::underlying_type<T>::type>(result));
}

template<typename T>
[[gnu::artificial]]
inline auto storeX86CallResult(JITThreadContext &context, T result) -> typename std::enable_if<std::is_class_v<T> && std::is_standard_layout_v<T> && sizeof(T) <= 16>::type {
    storeX86CallStructureResult(context, &result, sizeof(result));
}

template<int Position>
struct X86CallTranslateArgument {
    X86CallTranslateArgument(JITThreadContext &context) : context(context) {}

    template<typename T>
    [[gnu::artificial]]
    inline operator T() const {
        return retrieveX86CallArgument<T>(context, Position);
    }

private:
    JITThreadContext &context;
};

template<typename ReturnType, typename FunctionPointer, typename... Args>
struct X86CallAndTranslateResult {

    [[gnu::artificial]]
    static inline void call(JITThreadContext &context, FunctionPointer callee, Args... args) {
        storeX86CallResult<ReturnType>(context, callee(args...));
    }
};

template<typename FunctionPointer, typename... Args>
struct X86CallAndTranslateResult<void, FunctionPointer, Args...> {

    [[gnu::artificial]]
    static inline void call(JITThreadContext &context, FunctionPointer callee, Args... args) {
        callee(args...);
    }
};

template<int ArgumentsLeftToTranslate, typename X86FunctionPointer>
struct X86CallTranslatingFunctionApply {
    template<typename... AlreadyTranslatedArguments>
    [[gnu::artificial]]
    static inline void apply(JITThreadContext &context, X86FunctionPointer callee, AlreadyTranslatedArguments... args) {
        X86CallTranslatingFunctionApply<ArgumentsLeftToTranslate - 1, X86FunctionPointer>::apply(
            context,
            callee,
            X86CallTranslateArgument<ArgumentsLeftToTranslate - 1>(context),
            args...);
    }
};

template<typename X86FunctionPointer>
struct X86CallTranslatingFunctionApply<0, X86FunctionPointer> {
    template<typename... AlreadyTranslatedArguments>
    [[gnu::artificial]]
    static inline void apply(JITThreadContext &context, X86FunctionPointer callee, AlreadyTranslatedArguments... args) {
        using ReturnType = std::invoke_result_t<X86FunctionPointer, AlreadyTranslatedArguments...>;

        X86CallAndTranslateResult<ReturnType, X86FunctionPointer, AlreadyTranslatedArguments...>::call(context, callee, args...);
    }
};

template<typename ReturnType, typename... Args>
void x86call(ReturnType (*x86FunctionPointer)(Args... args)) noexcept {
    auto &context = JITThreadContext::get();
    try {
        X86CallTranslatingFunctionApply<sizeof...(Args), decltype(x86FunctionPointer)>::apply(context, x86FunctionPointer);
    } catch(const WrappedARMException &exception) {
        rethrowWrappedARMExceptionFromX86Call(exception);
    }
}

template<typename ReturnType, typename... Args>
void x86call() {
    return x86call(reinterpret_cast<ReturnType (*)(Args...)>(readThunkUtilitySlot()));
}

template<typename ReturnType, typename... Args>
static constexpr inline ThunkManager::X86ThunkTarget createTypedX86Thunk(ReturnType (*)(Args...)) {
    return static_cast<ThunkManager::X86ThunkTarget>(&x86call<ReturnType, Args...>);
}

#endif



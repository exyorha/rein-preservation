#include <Interop/PreparedInternalCall.h>
#include <Interop/ICallInterposerManager.h>
#include <Interop/InteropMethodLocator.h>

#include <ffi.h>
#include <il2cpp-api.h>
#include <il2cpp-tabledefs.h>

#include "support.h"

PreparedInternalCall::PreparedInternalCall(const std::string &fullName) {

    m_interposer = ICallInterposerManager::getInterposerForMethod(fullName);

    InteropMethodLocatorParameters parameters;
    parameters.mustBeInternalCall = true;

    auto methodDef = InteropMethodLocator::resolveMethod(fullName, parameters);

    uint32_t iflags;
    auto flags = il2cpp_method_get_flags(methodDef, &iflags);

    printf("PreparedInternalCall: preparing a thunk to call %s; il2cpp method %p; flags 0x%08X, iflags 0x%08X\n",
          fullName.c_str(), methodDef,
           flags, iflags);

    auto argumentCount = il2cpp_method_get_param_count(methodDef);

    size_t argumentsToReserve = argumentCount;

    bool hasThis = !(flags & METHOD_ATTRIBUTE_STATIC);

    if(hasThis)
        argumentsToReserve++;

    ARMArgumentPacker argumentPacker(argumentsToReserve);
    ARMArgumentPacker resultPacker(1);

    if(hasThis) {
        printf("the method is not static, getting the this pointer\n");

        argumentPacker.packPointer();

    }

    printf("  %u arguments\n", argumentCount);
    for(unsigned int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
        argumentPacker.pack(il2cpp_method_get_param(methodDef, argumentIndex));

    }

    resultPacker.pack(il2cpp_method_get_return_type(methodDef));

    if(m_interposer) {
        argumentPacker.reserveFFIOnlyArgument(&ffi_type_pointer);
    }

    m_arguments = std::move(argumentPacker.finish());
    m_result = resultPacker.finish().makeIntoSingleArgument();

    auto result = ffi_prep_cif(&m_cif,
                               FFI_DEFAULT_ABI,
                               m_arguments.size(),
                               m_result.type(),
                               m_arguments.types());

    if(result != FFI_OK) {
        panic("ffi_prep_cif has failed: %d\n", result);
    }

}

#if 0
else if(fullName == "UnityEngine.AndroidJNI::FindClass") {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(0));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Java: %s: '%s'\n", fullName.c_str(), conv.c_str());
    } else if(fullName == "UnityEngine.AndroidJNI::GetStaticMethodID" || fullName == "UnityEngine.AndroidJNI::GetMethodID" ||
              fullName == "UnityEngine.AndroidJNI::GetFieldID" || fullName == "UnityEngine.AndroidJNI::GetStaticFieldID"
    ) {
        auto url = static_cast<Il2CppString **>(argumentPointers.at(1));
        printf("string at (probably) %p\n", *url);
        auto length = il2cpp_string_length(*url);
        auto chars = il2cpp_string_chars(*url);
        printf("length %p, chars %u\n", chars, length);
        std::string conv;
        for(size_t index = 0; index < length; index++) {
            conv.push_back(chars[index]);
        }

        printf("Java: '%s': '%s'\n", fullName.c_str(), conv.c_str());

    }
#endif

PreparedInternalCall::~PreparedInternalCall() = default;

void PreparedInternalCall::invokeInternal(Il2CppMethodPointer method, JITThreadContext &context, SavedICallContext *contextWithInputArgs) const {

    std::vector<void *> args;
    if(contextWithInputArgs) {
        args = m_arguments.getPointers(*contextWithInputArgs);
    } else {
        args = m_arguments.getPointers(context);
    }

    auto returnPointer = m_result.getPointer(context);

    if(m_interposer) {
        args.push_back(&method);

        ffi_call(
            &m_cif,
            m_interposer,
            returnPointer,
            args.data());
    } else {

        ffi_call(
            &m_cif,
            method,
            returnPointer,
            args.data());
    }

#if 0
    if(fullName == "UnityEngine.AndroidJNI::FindClass" || fullName == "UnityEngine.AndroidJNI::NewGlobalRef") {
        static unsigned char fake;
        *reinterpret_cast<void **>(returnPointer) = &fake;
        printf("Java: faked the class, R0 = %016" PRIx64 "\n", thread.gprs[0]);
    } else if(fullName == "UnityEngine.AndroidJNI::ExceptionOccurred") {
        printf("Java: exception occurred: %016" PRIx64 "\n", thread.gprs[0]);
    }
#endif
}

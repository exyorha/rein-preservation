#include <Interop/PreparedInternalCall.h>
#include <Interop/ICallInterposerManager.h>
#include <Interop/InteropMethodLocator.h>


#include <ffi.h>
#include <il2cpp-api.h>
#include <il2cpp-tabledefs.h>

#include "support.h"

PreparedInternalCall::PreparedInternalCall(const std::string &fullName) {

    m_interposer = ICallInterposerManager::getInterposerForMethod(fullName);

    if(!m_interposer && fullName.starts_with("UnityEngine.AndroidJNI::")) {
        fprintf(stderr, "\nPreparedInternalCall: an internal call of '%s' is going to the native implementation without an interposer. This is probably an error, since this interface is not implemented in the Unity versions we're targeting.\n\n",
                fullName.c_str());
    }

    InteropMethodLocatorParameters parameters;
    parameters.mustBeInternalCall = true;

    auto methodDef = InteropMethodLocator::resolveMethod(fullName, parameters);

    uint32_t iflags;
    auto flags = il2cpp_method_get_flags(methodDef, &iflags);

#if 0
    printf("PreparedInternalCall: preparing a thunk to call %s; il2cpp method %p; flags 0x%08X, iflags 0x%08X\n",
          fullName.c_str(), methodDef,
           flags, iflags);
#endif

    auto argumentCount = il2cpp_method_get_param_count(methodDef);

    size_t argumentsToReserve = argumentCount;

    bool hasThis = !(flags & METHOD_ATTRIBUTE_STATIC);

    if(hasThis)
        argumentsToReserve++;

    ARMArgumentPacker argumentPacker(argumentsToReserve);
    ARMArgumentPacker resultPacker(1);

    if(hasThis) {
#if 0
        printf("the method is not static, getting the this pointer\n");
#endif
        argumentPacker.packPointer();

    }

#if 0
    printf("  %u arguments\n", argumentCount);
#endif
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

PreparedInternalCall::~PreparedInternalCall() = default;

void PreparedInternalCall::invokeInternal(Il2CppMethodPointer method, JITThreadContext &context, SavedICallContext *contextWithInputArgs) const {

    void **argsStart = reinterpret_cast<void **>(alloca(sizeof(void *) * m_arguments.size()));
    void **argsPtr = argsStart;

    if(contextWithInputArgs) {
        m_arguments.getPointers(*contextWithInputArgs, argsPtr);
    } else {
        m_arguments.getPointers(context, argsPtr);
    }

    auto returnPointer = m_result.getPointer(context);

    if(m_interposer) {
        *argsPtr++ = &method;

        ffi_call(
            &m_cif,
            m_interposer,
            returnPointer,
            argsStart);
    } else {

        ffi_call(
            &m_cif,
            method,
            returnPointer,
            argsStart);
    }
}

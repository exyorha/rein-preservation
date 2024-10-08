#include <Interop/MethodDiversion.h>
#include <Interop/ARMArgumentPacker.h>
#include <Interop/InteropCallFrame.h>

#include <Translator/DiversionManager.h>
#include <Translator/JITThreadContext.h>
#include <Translator/thunking.h>
#include <Translator/GCHooks.h>

#include <il2cpp-tabledefs.h>

#include "support.h"

MethodDiversion::MethodDiversion(const MethodInfo *methodDef, Il2CppMethodPointer interposer) : m_interposer(interposer) {
    uint32_t iflags;
    auto flags = il2cpp_method_get_flags(methodDef, &iflags);


    auto argumentCount = il2cpp_method_get_param_count(methodDef);

    size_t argumentsToReserve = argumentCount;

    bool hasThis = !(flags & METHOD_ATTRIBUTE_STATIC);

    if(hasThis)
        argumentsToReserve++;

    ARMArgumentPacker argumentPacker(argumentsToReserve);
    ARMArgumentPacker resultPacker(1);

    if(hasThis) {
        argumentPacker.packPointer();
    }

    for(unsigned int argumentIndex = 0; argumentIndex < argumentCount; argumentIndex++) {
        argumentPacker.pack(il2cpp_method_get_param(methodDef, argumentIndex));

    }

    argumentPacker.reserveFFIOnlyArgument(&ffi_type_pointer);

    resultPacker.pack(il2cpp_method_get_return_type(methodDef));

    m_arguments = std::move(argumentPacker.finish());
    m_result = resultPacker.finish().makeIntoSingleArgument();

    auto result = ffi_prep_cif(&m_cifForInterposerCall,
                               FFI_DEFAULT_ABI,
                               m_arguments.size(),
                               m_result.type(),
                               m_arguments.types());

    if(result != FFI_OK) {
        panic("ffi_prep_cif has failed: %d\n", result);
    }

    result = ffi_prep_cif(&m_cifForContinueCall,
                          FFI_DEFAULT_ABI,
                          m_arguments.size() - 1,
                          m_result.type(),
                          m_arguments.types());

    if(result != FFI_OK) {
        panic("ffi_prep_cif has failed: %d\n", result);
    }

    result = ffi_prep_closure_loc(m_closure.closure(), &m_cifForContinueCall, continueCall, this, m_closure.code());

    if(result != FFI_OK) {
        panic("ffi_prep_closure_loc has failed: %d\n", result);
    }
}

MethodDiversion::~MethodDiversion() = default;

MethodDiversion::AllocatedClosure::AllocatedClosure(size_t size) : m_closureData(ffi_closure_alloc(size, &m_closureCode)) {
    if(!m_closureData)
        throw std::bad_alloc();
}

MethodDiversion::AllocatedClosure::~AllocatedClosure() {
    ffi_closure_free(m_closureData);
}

void MethodDiversion::diversionHandler(const Diversion *diversion) {
    #ifndef CR_GARBAGE_COLLECT_HOST_STACKS
    /*
     * If we are not capable of marking the host stacks, then we need to
     * disable the ARM-side GC for the whole time the diversion is running.
     */
    GCDisablingScope disableGC;
#endif

    auto this_ = static_cast<MethodDiversion *>(diversion->userdata);

    auto &context = JITThreadContext::get();

    void **argsStart = reinterpret_cast<void **>(alloca(sizeof(void *) * this_->m_arguments.size()));
    void **argsPtr = argsStart;

    this_->m_arguments.getPointers(context, argsPtr);
    auto returnPointer = this_->m_result.getPointer(context);

    void *closurePtr = this_->m_closure.code();
    *argsPtr++ = &closurePtr;

    ffi_call(
        &this_->m_cifForInterposerCall,
        this_->m_interposer,
        returnPointer,
        argsStart);
}

void MethodDiversion::continueCall(ffi_cif *cif, void *ret, void **args, void *userData) {
    auto this_ = static_cast<MethodDiversion *>(userData);

    {
        InteropCallFrame callFrame(
            this_->m_arguments,
            this_->m_result,
            args,
            ret);

        auto &context = JITThreadContext::get();
        context.pc = this_->m_continueAddress;

        runARMCall(context);
    }
}

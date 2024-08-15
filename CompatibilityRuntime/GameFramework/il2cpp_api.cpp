#include <Interop/InteropMethodLocator.h>
#include <Interop/MethodDiversion.h>
#include <Interop/ICallImplementation.h>

#include <Translator/thunking.h>

#include <ELF/Image.h>

#include <translator_api.h>

#include "support.h"

#include "GlobalContext.h"
#include "il2cpp-api-internal.h"

#include <MiniGRPC/CompletionQueue.h>

static void (*postInitializeCallback)(void);

static Il2CppMethodPointer il2cpp_resolve_arm_icall(const char *name) {
    typedef void * (*FunctionPointer)(const char* name);

    static FunctionPointer arm_il2cpp_resolve_icall =
        reinterpret_cast<FunctionPointer>(GlobalContext::get().linkingSet().getSymbolChecked("il2cpp_resolve_icall"));

    return reinterpret_cast<Il2CppMethodPointer>(armcall(arm_il2cpp_resolve_icall, name));
}

void il2cpp_add_internal_call(const char* name, Il2CppMethodPointer method) {
    auto &context = GlobalContext::get();
    auto armImplementation = context.icallImplementation().installICallHandler(name, method);

    /*
     * If the ICall implementation manager can't supply us an implementation
     * (relevant for the Static manager - will return nullptr for icalls that
     * are not mentioned in the il2cpp binary), then we don't supply it to
     * il2cpp, and then it will raise NotImplementedException if a call to this
     * method somehow happens after all.
     */

    if(armImplementation) {
        typedef void(*FunctionPointer)(const char* name, void *method);

        static FunctionPointer arm_il2cpp_add_internal_call =
            reinterpret_cast<FunctionPointer>(GlobalContext::get().linkingSet().getSymbolChecked("il2cpp_add_internal_call"));

        armcall(arm_il2cpp_add_internal_call,
            name,
            armImplementation
        );
    }
}

Il2CppMethodPointer IL2CPP_EXPORT translator_resolve_native_icall(const char *name) {

    auto method = il2cpp_resolve_arm_icall(name);

    auto resolved = GlobalContext::get().icallImplementation().resolveNativeICall(name, reinterpret_cast<void *>(method));
    if(!resolved) {
        fprintf(stderr, "translator_resolve_native_icall is returning nullptr for '%s', is the name misspelled?\n", name);
    }

    return resolved;
}

Il2CppMethodPointer il2cpp_resolve_icall(const char* name) {
    panic("il2cpp_resolve_icall('%s')\n", name);
}

void translator_set_post_initialize_callback(void (*callback)(void)) {
    postInitializeCallback = callback;
}

int il2cpp_init(const char* domain_name) {

    auto result = internal_il2cpp_init(domain_name);
    if(result != 0 && postInitializeCallback) {
        postInitializeCallback();
    }

    return result;
}

int il2cpp_init_utf16(const Il2CppChar * domain_name) {

    auto result = internal_il2cpp_init_utf16(domain_name);
    if(result != 0 && postInitializeCallback) {
        postInitializeCallback();
    }

    return result;
}

void il2cpp_shutdown(void) {
    minigrpc::CompletionQueue::setGlobalShutdown();

    internal_il2cpp_shutdown();
}


void IL2CPP_EXPORT translator_divert_method(const char *methodName, Il2CppMethodPointer interposer) {

    InteropMethodLocatorParameters parameters;
    parameters.hasExplicitAssemblyName = true;
    auto method = InteropMethodLocator::resolveMethod(methodName, parameters);
    auto implementation = *reinterpret_cast<const Il2CppMethodPointer *>(method);

    auto diversionData = std::make_unique<MethodDiversion>(method, interposer);

    auto diversion =
        GlobalContext::get().diversionManager().divert(reinterpret_cast<void *>(implementation), MethodDiversion::diversionHandler, diversionData.release());

    static_cast<MethodDiversion *>(diversion->userdata)->setContinueAddress(diversion->continueThunkAddress());
}


/*
 * This is exactly what ARM code does, but avoids the overhead of an armcall
 * on a very hot function.
 */
void il2cpp_gc_wbarrier_set_field(Il2CppObject * obj, void **targetAddress, void *object) {
    (void)obj;

    /*
     * Make sure that this thread gets the JIT context.
     */
    *targetAddress = object;
}

void translator_disallow_precompiled_icall_use(void) {
    GlobalContext::precompiledICallUseDisallowed = true;
}

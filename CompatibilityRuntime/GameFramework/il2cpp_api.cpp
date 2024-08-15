#include <Interop/InternalCallThunk.h>
#include <Interop/InteropMethodLocator.h>
#include <Interop/MethodDiversion.h>
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
    auto thunkContext = std::make_unique<InternalCallThunk>(name, method);

    typedef void(*FunctionPointer)(const char* name, void *method);

    static FunctionPointer arm_il2cpp_add_internal_call =
        reinterpret_cast<FunctionPointer>(GlobalContext::get().linkingSet().getSymbolChecked("il2cpp_add_internal_call"));

    auto thunk = GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(thunkContext.release(), &InternalCallThunk::thunkCall);

    armcall(arm_il2cpp_add_internal_call,
        name,
        thunk
    );
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

Il2CppMethodPointer IL2CPP_EXPORT translator_resolve_native_icall(const char *name) {

    auto method = il2cpp_resolve_arm_icall(name);
    if(!method) {
        return nullptr;
    }

    auto thunk = static_cast<InternalCallThunk *>(GlobalContext::get().thunkManager().lookupARMToX86ThunkCall(reinterpret_cast<void *>(method)));
    if(!thunk)
        panic("translator_resolve_native_icall(%s): the ARM-side handle, %p, doesn't correspond to a thunk\n",
              name, reinterpret_cast<void *>(method));

    return thunk->x86Method();
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

#include <ICall/InternalCallThunk.h>

#include <translator_api.h>

#include "support.h"
#include <Translator/thunking.h>
#include <ELF/Image.h>

#include "GlobalContext.h"

static void (*postInitializeCallback)(void);

void il2cpp_add_internal_call(const char* name, Il2CppMethodPointer method) {
    auto thunkContext = std::make_unique<InternalCallThunk>(name, method);

    typedef void(*FunctionPointer)(const char* name, void *method);

    static FunctionPointer arm_il2cpp_add_internal_call =
        reinterpret_cast<FunctionPointer>(GlobalContext::get().il2cpp().getSymbolChecked("il2cpp_add_internal_call"));

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

    typedef int(*FunctionPointer)(const char* domain_name);

    static FunctionPointer arm_il2cpp_init = reinterpret_cast<FunctionPointer>(GlobalContext::get().il2cpp().getSymbolChecked("il2cpp_init"));

    auto result = armcall(arm_il2cpp_init, domain_name);
    if(result != 0 && postInitializeCallback) {
        postInitializeCallback();
    }

    return result;
}

int il2cpp_init_utf16(const Il2CppChar * domain_name) {

    typedef int(*FunctionPointer)(const Il2CppChar * domain_name);

    static FunctionPointer arm_il2cpp_init_utf16 = reinterpret_cast<FunctionPointer>(GlobalContext::get().il2cpp().getSymbolChecked("il2cpp_init_utf16"));

    auto result = armcall(arm_il2cpp_init_utf16, domain_name);
    if(result != 0 && postInitializeCallback) {
        postInitializeCallback();
    }

    return result;
}

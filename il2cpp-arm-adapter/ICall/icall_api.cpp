#include <ICall/InternalCallThunk.h>

#include "support.h"
#include <Translator/thunking.h>
#include <ELF/Image.h>

void il2cpp_add_internal_call(const char* name, Il2CppMethodPointer method) {
    auto thunkContext = std::make_unique<InternalCallThunk>(name, method);

    typedef void(*FunctionPointer)(const char* name, void *method);

    static FunctionPointer arm_il2cpp_add_internal_call =
        reinterpret_cast<FunctionPointer>(Image::get_il2cpp_image()->getSymbolChecked("il2cpp_add_internal_call"));

    auto thunk = ThunkManager::allocateARMToX86ThunkCall(thunkContext.release(), &InternalCallThunk::thunkCall);

    armcall(arm_il2cpp_add_internal_call,
        name,
        thunk
    );
}

Il2CppMethodPointer il2cpp_resolve_icall(const char* name) {
    panic("il2cpp_resolve_icall('%s')\n", name);
}


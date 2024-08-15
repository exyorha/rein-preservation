#include <Interop/ICallImplementationDynamic.h>
#include <Interop/InternalCallThunk.h>

#include "GlobalContext.h"
#include "support.h"

ICallImplementationDynamic::ICallImplementationDynamic() = default;

ICallImplementationDynamic::~ICallImplementationDynamic() = default;

void *ICallImplementationDynamic::installICallHandler(const char *name, Il2CppMethodPointer implementation) {

    auto thunkContext = std::make_unique<InternalCallThunk>(name, implementation);

    return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(thunkContext.release(), &InternalCallThunk::thunkCall);
}

Il2CppMethodPointer ICallImplementationDynamic::resolveNativeICall(const char *name, void *handler) {
    (void)name;

    if(!handler)
        return nullptr;

    auto thunk = static_cast<InternalCallThunk *>(GlobalContext::get().thunkManager().lookupARMToX86ThunkCall(reinterpret_cast<void *>(handler)));
    if(!thunk)
        panic("translator_resolve_native_icall(%s): the ARM-side handle, %p, doesn't correspond to a thunk\n",
              name, reinterpret_cast<void *>(handler));

    return thunk->x86Method();
}

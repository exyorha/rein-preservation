#include "ICallImplementationStatic.h"
#include "ICallInterposerManager.h"
#include "GlobalContext.h"

#include <Translator/thunking.h>
#include <Translator/JITThreadContext.h>

ICallImplementationStatic::ICallImplementationStatic(const std::filesystem::path &libraryPath) : m_thunkLibrary(libraryPath.c_str()) {
    m_lookup = reinterpret_cast<LookupPrecompiledICallThunkFunc>(m_thunkLibrary.getProcAddress("get_precompiled_icall_thunks"));
    if(!m_lookup)
        throw std::runtime_error("the thunk library doesn't implement get_precompiled_icall_thunks");
}

ICallImplementationStatic::~ICallImplementationStatic() = default;

void *ICallImplementationStatic::installICallHandler(const char *name, Il2CppMethodPointer implementation) {
    auto thunks = m_lookup(name);
    if(!thunks) {
        std::unique_lock<std::mutex> locker(m_unknownICallTableMutex);
        m_unknownICallTable.emplace(name, implementation);
        return nullptr;
    }

    *thunks->icallHandler = implementation;

    auto interposer = ICallInterposerManager::getInterposerForMethod(name);

    *thunks->icallInterposer = interposer;

    void *thunkProc;

    if(interposer) {
        thunkProc = reinterpret_cast<void *>(thunks->withInterposer);
    } else {
        thunkProc = reinterpret_cast<void *>(thunks->withoutInterposer);
    }

    return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(thunkProc, &thunkCall);
}

Il2CppMethodPointer ICallImplementationStatic::resolveNativeICall(const char *name, void *handler) {
    auto thunks = m_lookup(name);
    if(!thunks) {
        std::unique_lock<std::mutex> locker(m_unknownICallTableMutex);;
        auto it = m_unknownICallTable.find(name);
        if(it != m_unknownICallTable.end()) {
            return it->second;
        }

        return nullptr;
    }

    return *thunks->icallHandler;
}

void ICallImplementationStatic::thunkCall() noexcept {

    auto thunk = reinterpret_cast<ICallThunk>(readThunkUtilitySlot());

    try {
        thunk(JITThreadContext::get());
    } catch(const WrappedARMException &exception) {
        rethrowWrappedARMExceptionFromX86Call(exception);
    }
}

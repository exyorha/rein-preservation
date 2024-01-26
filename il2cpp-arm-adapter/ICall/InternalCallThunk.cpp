#include <ICall/InternalCallThunk.h>

#include <cstdio>

#include <Translator/JITThreadContext.h>
#include <Translator/thunking.h>

InternalCallThunk::InternalCallThunk(const char *name, Il2CppMethodPointer x86Method) :
    m_call(std::in_place_type_t<std::string>(), name),
    m_x86Method(x86Method) {

}

InternalCallThunk::~InternalCallThunk() = default;

void InternalCallThunk::thunkCall() {
    static_cast<InternalCallThunk *>(thunkUtilitySlot)->execute();
}

const PreparedInternalCall *InternalCallThunk::prepareInternalCall(std::string &&name, std::optional<SavedICallContext> &savedContext) {
    /*
     * This implements one-time - done during the first call of the thunk -
     * preparation needed to call the host platform's implementation of this
     * icall.
     *
     * All the following calls are just ffi_call's.
     */

    /*
     * Copy the input arguments, since we're going to trash them by calling other ARM functions.
     */
    auto &thread = JITThreadContext::get();
    savedContext.emplace(thread);

    /*
     * The name is stored in the same variant we're going to reinitialize, so we need to move it out.
     */
    auto nameCopy = std::move(name);
    return &m_call.emplace<PreparedInternalCall>(std::move(nameCopy));
}

void InternalCallThunk::execute() {
    /*
     * First, try to take the fast path: take a shared lock and see if the
     * thunk is already prepared. If it is, then call immediately - it'll be
     * just a ffi_call that can be done in parallel without any trouble.
     *
     * If not, take the slow path of stashing the ARM thread context,
     * taking the unique lock, preparing the thunk (if needed: we're
     * double-locking, so the thunk could be prepared by someone else in the
     * unlocked meantime), then re-locking into the shared lock, and finally
     * calling via FFI.
     */

    const PreparedInternalCall *preparedCall = nullptr;

    {
        std::shared_lock<std::shared_mutex> sharedLock(m_thunkMutex);

        preparedCall = std::get_if<PreparedInternalCall>(&m_call);
    }

    std::optional<SavedICallContext> savedContext;

    if(!preparedCall) {
        // Stash context here.

        std::unique_lock<std::shared_mutex> uniqueLock(m_thunkMutex);

        preparedCall = std::visit([this, &savedContext](auto &&value) -> const PreparedInternalCall * {
            return prepareInternalCall(std::move(value), savedContext);

        }, m_call);
    }

    auto &thread = JITThreadContext::get();
    if(savedContext.has_value()) {
        preparedCall->invoke(m_x86Method, thread, *savedContext);
    } else {
        preparedCall->invoke(m_x86Method, thread);
    }

}


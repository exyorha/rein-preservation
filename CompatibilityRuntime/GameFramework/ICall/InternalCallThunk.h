#ifndef INTERNAL_CALL_THUNK_H
#define INTERNAL_CALL_THUNK_H

#include <il2cpp-api.h>
#include <string>
#include <shared_mutex>
#include <variant>
#include <optional>

#include <ffi.h>

#include <ICall/PreparedInternalCall.h>
#include <ICall/SavedICallContext.h>

class InternalCallThunk {
public:
    InternalCallThunk(const char *name, Il2CppMethodPointer x86Method);
    ~InternalCallThunk();

    InternalCallThunk(const InternalCallThunk &other) = delete;
    InternalCallThunk &operator =(const InternalCallThunk &other) = delete;

    static void thunkCall();

private:

    void execute();

    static inline const PreparedInternalCall *prepareInternalCall(const PreparedInternalCall &prepared,
                                                                  std::optional<SavedICallContext> &savedContext) {
        (void)savedContext;
        return &prepared;
    }

    const PreparedInternalCall *prepareInternalCall(std::string &&name,
                                                    std::optional<SavedICallContext> &savedContext);

    std::shared_mutex m_thunkMutex;
    std::variant<std::string, PreparedInternalCall> m_call;
    Il2CppMethodPointer m_x86Method;
};


#endif

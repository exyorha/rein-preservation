#ifndef PREPARED_INTERNAL_CALL_H
#define PREPARED_INTERNAL_CALL_H

#include <string>
#include <vector>
#include <cstdint>
#include <variant>

#include <Interop/ARMArgumentPacker.h>

class JITThreadContext;
class SavedICallContext;

class PreparedInternalCall {
public:
    explicit PreparedInternalCall(const std::string &name);
    ~PreparedInternalCall();

    PreparedInternalCall(const PreparedInternalCall &other) = delete;
    PreparedInternalCall &operator =(const PreparedInternalCall &other) = delete;

    inline void invoke(Il2CppMethodPointer method, JITThreadContext &context) const {
        return invokeInternal(method, context, nullptr);
    }

    inline void invoke(Il2CppMethodPointer method, JITThreadContext &context, SavedICallContext &contextWithInputArgs) const {
        return invokeInternal(method, context, &contextWithInputArgs);
    }

private:
    void invokeInternal(Il2CppMethodPointer method, JITThreadContext &context, SavedICallContext *contextWithInputArgs) const;

    Il2CppMethodPointer m_interposer;
    ARMArgumentSet m_arguments;
    ARMSingleArgument m_result;
    mutable ffi_cif m_cif;
};

#endif

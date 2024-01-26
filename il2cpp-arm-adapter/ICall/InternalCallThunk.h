#ifndef INTERNAL_CALL_THUNK_H
#define INTERNAL_CALL_THUNK_H

#include <il2cpp-api.h>
#include <string>

#include <ffi.h>

class InternalCallThunk {
public:
    InternalCallThunk(const char *name, Il2CppMethodPointer x86Method);
    ~InternalCallThunk();

    InternalCallThunk(const InternalCallThunk &other) = delete;
    InternalCallThunk &operator =(const InternalCallThunk &other) = delete;

    static void thunkCall();

private:

    enum class ValueCategory {
        Integer,
        Void,
        Vector
    };

    void execute();

    static ValueCategory getValueCategory(const Il2CppType *type, ffi_type **ffi);

    std::string m_name;
    Il2CppMethodPointer m_x86Method;
};


#endif

#ifndef ICALL_IMPLEMENTATION_H
#define ICALL_IMPLEMENTATION_H

#include <il2cpp-api.h>

class ICallImplementation {
protected:
    ICallImplementation();

public:
    virtual ~ICallImplementation();

    ICallImplementation(const ICallImplementation &other) = delete;
    ICallImplementation &operator =(const ICallImplementation &other) = delete;

    /*
     * Receives the Unity ICall handler named 'name', and the Unity x86 handler in 'implementation'.
     * Records the 'implementation' internally, and returns the ARM method pointer that is suitable
     * to be passed to the ARM il2cpp_add_internal_call.
     */
    virtual void *installICallHandler(const char *name, Il2CppMethodPointer implementation) = 0;

    /*
     * Receives the name of the icall in 'name' and the previously set ARM handler in handler,
     * and returns the previously set Unity x86 handler, or nullptr if non set or known.
     */
    virtual Il2CppMethodPointer resolveNativeICall(const char *name, void *handler) = 0;
};

#endif

#ifndef PRECOMPILED_ICALL_INTERFACE_H
#define PRECOMPILED_ICALL_INTERFACE_H

struct ARMMachineContext;

typedef void (*ICallThunk)(ARMMachineContext &ctx);

typedef void (*ICallTypeErasedTarget)(void);

struct ICallPrecompiledThunks {
    ICallThunk withoutInterposer;
    ICallThunk withInterposer;
    ICallTypeErasedTarget *icallHandler;
    ICallTypeErasedTarget *icallInterposer;
};

// Exported by the thunk library under 'get_precompiled_icall_thunks'.
typedef const ICallPrecompiledThunks *(*LookupPrecompiledICallThunkFunc)(const char *icallName);

#endif

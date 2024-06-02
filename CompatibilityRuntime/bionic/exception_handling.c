#include <unwind.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>

extern void __attribute__((noreturn)) __compatibility_runtime_armcall_exception(void);
void __attribute__((noreturn)) __compatibility_runtime_armcall_rethrower(_Unwind_Exception *exception);

void (*__compatibility_runtime_actual_Unwind_SetIP)(struct _Unwind_Context *, unsigned long) = NULL;
void (*__compatibility_runtime_actual_Unwind_SetGR)(struct _Unwind_Context *, int, unsigned long) = NULL;
_Unwind_Reason_Code (*__compatibility_runtime_actual_Unwind_RaiseException)(struct _Unwind_Exception *) = NULL;

static void unwinderError(const char *message) {
    write(STDERR_FILENO, message, strlen(message));
}

_Unwind_Reason_Code __compatibility_runtime_armcall_personality(
    int version,
    _Unwind_Action action,
    _Unwind_Exception_Class exceptionClass,
    _Unwind_Exception *exception,
    struct _Unwind_Context *context) {

    if(version != 1) {
        unwinderError("armcall_personality_routine called with version other than 1\n");
        return _URC_FATAL_PHASE1_ERROR;
    }

    if(action & _UA_FORCE_UNWIND) {
        unwinderError("armcall_personality_routine: cannot do forced unwinding (longjmp, etc) across native frames\n");

        return _URC_FATAL_PHASE1_ERROR;
    }

    if(action & _UA_SEARCH_PHASE) {
        /*
         * We handle any and all exceptions.
         */
        return _URC_HANDLER_FOUND;
    } else if(action & _UA_HANDLER_FRAME) {
        /*
         * We're being called to handle an exception.
         */
        __compatibility_runtime_actual_Unwind_SetIP(context, (_Unwind_Word)__compatibility_runtime_armcall_exception);
        __compatibility_runtime_actual_Unwind_SetGR(context, 0, exceptionClass);
        __compatibility_runtime_actual_Unwind_SetGR(context, 1, (_Unwind_Word)exception);

        return _URC_INSTALL_CONTEXT;


    } else {
        unwinderError("armcall_personality_routine: bogus unwinding action; note that it's not possible to *clean up* native frames, all exceptions must be caught at the boundary\n");

        return _URC_FATAL_PHASE2_ERROR;
    }
    return _URC_NO_REASON;
}

void __compatibility_runtime_armcall_rethrower(_Unwind_Exception *exception) {
    __compatibility_runtime_actual_Unwind_RaiseException(exception);
    unwinderError("__compatibility_runtime_armcall_rethrower: _Unwind_RaiseException has failed\n");
    abort();
}

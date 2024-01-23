#ifndef THUNKING_H
#define THUNKING_H

#include "support.h"

template<typename ReturnType, typename... Args>
ReturnType armcall(ReturnType (*armFunctionPointer)(Args... args), Args... args) {
    stubcall("armcall not implemented\n");
}

template<typename ReturnType, typename... Args>
[[noreturn]] void armcall_noreturn(ReturnType (*armFunctionPointer)(Args... args), Args... args) {
    stubcall("armcall_noreturn not implemented\n");
}

#endif



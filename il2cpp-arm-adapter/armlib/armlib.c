typedef __builtin_va_list va_list;
typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned long uintptr_t;
typedef unsigned char uint8_t;
typedef unsigned long size_t;

#define va_start(l,a) __builtin_va_start(l,a)
#define va_end(l) __builtin_va_end(l)
#define va_arg(l,a) __builtin_va_arg(l,a)

#define NANOPRINTF_IMPLEMENTATION 1
#define NANOPRINTF_VISIBILITY_STATIC 1
#include "nanoprintf.h"

int vsprintf(char *dest, const char *format, va_list args) {
    return npf_vsnprintf(dest, (size_t)-1, format, args);
}

int vsnprintf(char *dest, size_t size, const char *format, va_list args) {
    return npf_vsnprintf(dest, size, format, args);
}

int sprintf(char *dest, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = vsprintf(dest, format, args);

    va_end(args);

    return result;
}

int snprintf(char *dest, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);

    int result = vsnprintf(dest, size, format, args);

    va_end(args);

    return result;
}

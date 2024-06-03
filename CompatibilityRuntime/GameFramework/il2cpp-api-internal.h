#ifndef IL2CPP_API_INTERNAL_H
#define IL2CPP_API_INTERNAL_H

#include <il2cpp-api.h>

#if defined(__cplusplus)
extern "C" {
#endif


int internal_il2cpp_init(const char* domain_name);
int internal_il2cpp_init_utf16(const Il2CppChar * domain_name);
void internal_il2cpp_shutdown(void);

#if defined(__cplusplus)
}
#endif

#endif


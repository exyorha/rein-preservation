#include <il2cpp-api.h>
#include <cstdio>

void il2cpp_add_internal_call(const char* name, Il2CppMethodPointer method) {
    printf("il2cpp_add_internal_call('%s', %p)\n", name, method);
}

Il2CppMethodPointer il2cpp_resolve_icall(const char* name) {
    printf("il2cpp_resolve_icall('%s')\n", name);

    return nullptr;
}


#ifndef TRANSLATOR_API_H
#define TRANSLATOR_API_H

#include "il2cpp-api-types.h"
#include <il2cpp_export.h>
#include <il2cpp-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*TranslatorEngineMain)(void *arg);

int IL2CPP_EXPORT translator_main(TranslatorEngineMain engineMain, void *arg);

void IL2CPP_EXPORT translator_add_icall_interposer(const char *name, Il2CppMethodPointer interposer);
void IL2CPP_EXPORT translator_remove_icall_interposer(const char *name);

Il2CppMethodPointer IL2CPP_EXPORT translator_resolve_native_icall(const char *name);

void IL2CPP_EXPORT translator_set_post_initialize_callback(void (*callback)(void));

IL2CPP_EXPORT void * translator_get_il2cpp_base_address(void);
void IL2CPP_EXPORT translator_flush_instruction_cache(void *baseAddress, size_t size);

void IL2CPP_EXPORT translator_divert_method(const char *method, Il2CppMethodPointer interposer);

void IL2CPP_EXPORT translator_disallow_precompiled_icall_use(void);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

template<typename Return, typename... Args>
static inline void translator_divert_method(const char *method, Return (*interposer)(Args...)) {
    translator_divert_method(method, reinterpret_cast<Il2CppMethodPointer>(interposer));
}

#include <string_view>

class ICallInterposerRegistration {
public:

    inline ICallInterposerRegistration(const char *name, Il2CppMethodPointer interposer) {
        translator_add_icall_interposer(name, interposer);
    }

    template<typename ReturnType, typename... Args>
    inline ICallInterposerRegistration(const char *name, ReturnType (*interposer)(Args... args)) :
        ICallInterposerRegistration(name, reinterpret_cast<Il2CppMethodPointer>(interposer)) {

    }

    inline ~ICallInterposerRegistration() = default;

    ICallInterposerRegistration(const ICallInterposerRegistration &other) = delete;
    ICallInterposerRegistration &operator =(const ICallInterposerRegistration &other) = delete;
};

#define INTERPOSE_ICALL(name,function) static ICallInterposerRegistration function ## _Reg(name, function);

#endif

#endif

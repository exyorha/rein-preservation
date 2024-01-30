#ifndef TRANSLATOR_API_H
#define TRANSLATOR_API_H

#include <il2cpp_export.h>
#include <il2cpp-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*TranslatorEngineMain)(int argc, char **argv);

int IL2CPP_EXPORT translator_main(int argc, char **argv, TranslatorEngineMain engineMain);

void IL2CPP_EXPORT translator_add_icall_interposer(const char *name, Il2CppMethodPointer interposer);
void IL2CPP_EXPORT translator_remove_icall_interposer(const char *name);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

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

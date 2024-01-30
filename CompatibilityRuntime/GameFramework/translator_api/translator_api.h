#ifndef TRANSLATOR_API_H
#define TRANSLATOR_API_H

#include <il2cpp_export.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (*TranslatorEngineMain)(int argc, char **argv);

int IL2CPP_EXPORT translator_main(int argc, char **argv, TranslatorEngineMain engineMain);

#if defined(__cplusplus)
}
#endif

#endif

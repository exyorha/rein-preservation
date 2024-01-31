#include <translator_api.h>

#include <memory>

#include "GlobalContext.h"

int translator_main(int argc, char **argv, TranslatorEngineMain engineMain) {
    auto globalContext = std::make_unique<GlobalContext>();

    return engineMain(argc, argv);
}

void *translator_get_il2cpp_base_address() {
    return reinterpret_cast<void *>(GlobalContext::get().il2cpp().displacement());
}

void translator_flush_instruction_cache(void *baseAddress, size_t size) {
    GlobalContext::get().jit().flushInstructionCache(reinterpret_cast<uintptr_t>(baseAddress), size);
}

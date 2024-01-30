#include <translator_api.h>

#include <memory>

#include "GlobalContext.h"

int translator_main(int argc, char **argv, TranslatorEngineMain engineMain) {
    auto globalContext = std::make_unique<GlobalContext>();

    return engineMain(argc, argv);
}

#include <Bionic/BionicDynamicLoaderAPI.h>

#include <cstdio>

#include "GlobalContext.h"
#include "SystemAPIThunking.h"

int emulated_dladdr(const void *addr, bionic_Dl_info *info) {
    return 0;
}

int emulated_dlclose(void *handle) {
    printf("emulated_dlclose: %p\n", handle);
    return 0;
}

char *emulated_dlerror(void) {
    printf("emulated_dlerror\n");
    return nullptr;
}

typedef int (*bionic_dl_iterate_phdr_callback)(struct bionic_dl_phdr_info *info, size_t size, void *data);

static int emulated_dl_iterate_phdr_module(const Image &image, bionic_dl_iterate_phdr_callback callback, void *data) {
    struct bionic_dl_phdr_info info;
    info.dlpi_addr = image.displacement();
    info.dlpi_name = image.path().filename().generic_string().c_str();
    info.dlpi_phdr = image.phdr();
    info.dlpi_phnum = image.phnum();

    return callback(&info, sizeof(info), data);
}

int emulated_dl_iterate_phdr(bionic_dl_iterate_phdr_callback callback, void *data) {

    auto result = emulated_dl_iterate_phdr_module(GlobalContext::get().armlib(), callback, data);
    if(result != 0)
        return result;

    return emulated_dl_iterate_phdr_module(GlobalContext::get().il2cpp(), callback, data);
}

void *emulated_dlopen(const char *filename, int flags) {
    static unsigned char pseudohandle;

    printf("emulated_dlopen(%s, %d)\n", filename, flags);

    return &pseudohandle;
}

void *emulated_dlsym(void *handle, const char *sym) {
    printf("emulated_dlsym(%p, %s)\n", handle, sym);

    void *value;
    if(GlobalContext::get().armlib().getSymbol(sym, value)) {
        return value;
    }

    return resolveUndefinedARMSymbol(sym);
}

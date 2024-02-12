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

int emulated_dl_iterate_phdr(int (*callback)(struct bionic_dl_phdr_info *info, size_t size, void *data), void *data) {
    /*
     * We report exactly one module - libil2cpp.so.
     */
    const auto &image = GlobalContext::get().il2cpp();

    struct bionic_dl_phdr_info info;
    info.dlpi_addr = image.displacement();
    info.dlpi_name = "libil2cpp.so";
    info.dlpi_phdr = image.phdr();
    info.dlpi_phnum = image.phnum();

    return callback(&info, sizeof(info), data);
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

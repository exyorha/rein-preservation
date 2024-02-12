#ifndef BIONIC_BIONIC_DYNAMIC_LOADER_API_H
#define BIONIC_BIONIC_DYNAMIC_LOADER_API_H

#include <Bionic/BionicABITypes.h>

int emulated_dladdr(const void *addr, bionic_Dl_info *info);

int emulated_dlclose(void *handle);

char *emulated_dlerror(void);

int emulated_dl_iterate_phdr(int (*callback)(struct bionic_dl_phdr_info *info, size_t size, void *data), void *data);

void *emulated_dlopen(const char *filename, int flags);

void *emulated_dlsym(void *handle, const char *sym);

#endif

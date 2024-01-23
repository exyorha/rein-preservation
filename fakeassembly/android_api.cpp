#include "android_api.h"

#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>

void android_log_print_raw_thunk(void) {
    printf("android_log_print_raw_thunk\n");
}

void android_set_abort_message(const char *message) {
    printf("android_set_abort_message(%s)\n", message);
}

int emulated_cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) {
    printf("emulated_cxa_atexit: %p, %p, %p\n", func, arg, dso_handle);
    return 0;
}

void emulated_cxa_finalize(void * d) {
    printf("emulated_cxa_finalize: %p\n", d);
}

int emulated_dladdr(const void *addr, android_Dl_info *info) {
    printf("emulated_dladdr: %p, %p\n", addr, info);
    return 0;
}

int emulated_dlclose(void *handle) {
    printf("emulated_dlcose: %p\n", handle);
    return 0;
}

char *emulated_dlerror(void) {
    printf("emulated_dlerror\n");
    return nullptr;
}

int emulated_dl_iterate_phdr(int (*callback)(struct android_dl_phdr_info *info, size_t size, void *data), void *data) {
    printf("emulated_dl_iterate_phdr(%p, %p)\n", callback, data);
    return 0;
}

void *emulated_dlopen(const char *filename, int flags) {
    printf("emulated_dlopen(%p, %d)\n", filename, flags);
    return nullptr;
}

void *emulated_dlsym(void *handle, const char *sym) {
    printf("emulated_dlsym(%p, %s)\n", handle, sym);
    return nullptr;
}

void android_FD_SET_chk(int fd, fd_set *set, size_t size) {
    printf("android_FD_SET_chk(%d, %p, %zu)\n", fd, set, size);
}

int android_system_property_get(const char *name, char *value) {
    printf("android_system_property_get(%s)\n", name);
    *value = 0;

    return 0;
}

void arm_setjmp() {
    printf("arm_setjmp\n");
}

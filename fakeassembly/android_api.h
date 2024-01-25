#ifndef ANDROID_API_H
#define ANDROID_API_H

#include <cstring>
#include <sys/select.h>
#include <sys/types.h>

void android_log_print_raw_thunk(void);
void android_set_abort_message(const char *message);

int emulated_cxa_atexit(void (*func) (void *), void * arg, void * dso_handle);
void emulated_cxa_finalize(void * d);

struct android_Dl_info;

int emulated_dladdr(const void *addr, android_Dl_info *info);

int emulated_dlclose(void *handle);

char *emulated_dlerror(void);

struct android_dl_phdr_info;

int emulated_dl_iterate_phdr(int (*callback)(struct android_dl_phdr_info *info, size_t size, void *data), void *data);

void *emulated_dlopen(const char *filename, int flags);

void *emulated_dlsym(void *handle, const char *sym);

void android_FD_SET_chk(int fd, fd_set *set, size_t size);

int android_system_property_get(const char *name, char *value);

struct android_stat;

int android_stat(const char *pathname, struct android_stat *statbuf);
int android_lstat(const char *pathname, struct android_stat *statbuf);
int android_fstat(int fd, struct android_stat *statbuf);

#endif

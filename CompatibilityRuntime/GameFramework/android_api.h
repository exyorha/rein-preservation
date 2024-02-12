#ifndef ANDROID_API_H
#define ANDROID_API_H

#include <cstring>
#include <sys/select.h>
#include <sys/types.h>

struct android_Dl_info;

int emulated_dladdr(const void *addr, android_Dl_info *info);

int emulated_dlclose(void *handle);

char *emulated_dlerror(void);

struct android_dl_phdr_info;

int emulated_dl_iterate_phdr(int (*callback)(struct android_dl_phdr_info *info, size_t size, void *data), void *data);

void *emulated_dlopen(const char *filename, int flags);

void *emulated_dlsym(void *handle, const char *sym);

int android_system_property_get(const char *name, char *value);

struct android_stat;

int android_stat(const char *pathname, struct android_stat *statbuf);
int android_lstat(const char *pathname, struct android_stat *statbuf);
int android_fstat(int fd, struct android_stat *statbuf);
int android_fstatat(int fd, const char *path, struct android_stat *statbuf, int flag);

#endif

#include "android_api.h"
#include <ELF/Image.h>
#include "SystemAPIThunking.h"
#include "GlobalContext.h"
#include "Translator/JITThreadContext.h"

#include <elf.h>
#include <signal.h>

#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <Translator/JIT.h>
#include <Translator/ThunkManager.h>
#include <Translator/thunking.h>

typedef uint32_t android_nlink_t;

struct android_stat {
  dev_t st_dev;
  ino_t st_ino;
  mode_t st_mode;
  android_nlink_t st_nlink;
  uid_t st_uid;
  gid_t st_gid;
  dev_t st_rdev;
  unsigned long __pad1;
  off_t st_size;
  int st_blksize;
  int __pad2;
  long st_blocks;
  struct timespec st_atim;
  struct timespec st_mtim;
  struct timespec st_ctim;
  unsigned int __unused4;
  unsigned int __unused5;

};

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

struct android_dl_phdr_info {
  Elf64_Addr dlpi_addr;
  const char* dlpi_name;
  const Elf64_Phdr* dlpi_phdr;
  Elf64_Half dlpi_phnum;
};

int emulated_dl_iterate_phdr(int (*callback)(struct android_dl_phdr_info *info, size_t size, void *data), void *data) {
    /*
     * We report exactly one module - libil2cpp.so.
     */
    const auto &image = GlobalContext::get().il2cpp();

    struct android_dl_phdr_info info;
    info.dlpi_addr = image.displacement();
    info.dlpi_name = "libil2cpp.so";
    info.dlpi_phdr = image.phdr();
    info.dlpi_phnum = image.phnum();

    return callback(&info, sizeof(info), data);
}

static unsigned char nocallHandle;

static void dummyThunk() {
    //printf("Crashlytics function was called\n");
    JITThreadContext::get().gprs[0] = 0;
}

void *emulated_dlopen(const char *filename, int flags) {
    static unsigned char pseudohandle;

    printf("emulated_dlopen(%s, %d)\n", filename, flags);

    if(std::string_view(filename).find("FirebaseCppApp") != std::string_view::npos)
        return &nocallHandle;

    return &pseudohandle;
}

void *emulated_dlsym(void *handle, const char *sym) {
    printf("emulated_dlsym(%p, %s)\n", handle, sym);

    if(handle == &nocallHandle) {
        return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(dummyThunk), dummyThunk);
    }

    return resolveUndefinedARMSymbol(sym);
}

void android_FD_SET_chk(int fd, fd_set *set, size_t size) {
    printf("android_FD_SET_chk(%d, %p, %zu)\n", fd, set, size);
}

int android_system_property_get(const char *name, char *value) {
    printf("android_system_property_get(%s)\n", name);
    *value = 0;

    return 0;
}

static void translateStat(const struct stat *native, struct android_stat *android) {
    android->st_dev = native->st_dev;
    android->st_ino = native->st_ino;
    android->st_mode = native->st_mode;
    android->st_nlink = native->st_nlink;
    android->st_uid = native->st_uid;
    android->st_gid = native->st_gid;
    android->st_rdev = native->st_rdev;
    android->st_size = native->st_size;
    android->st_blksize = native->st_blksize;
    android->st_blocks = native->st_blocks;
    android->st_atim = native->st_atim;
    android->st_mtim = native->st_mtim;
    android->st_ctim = native->st_ctim;
}

int android_stat(const char *pathname, struct android_stat *statbuf) {
    struct stat native;

    auto result = ::stat(pathname, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int android_lstat(const char *pathname, struct android_stat *statbuf) {
    struct stat native;

    auto result = ::lstat(pathname, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int android_fstat(int fd, struct android_stat *statbuf) {
    struct stat native;

    auto result = ::fstat(fd, &native);
    if(result >= 0) {
        translateStat(&native, statbuf);
    }

    return result;
}

int android_sysconf(int token) {
    int translated;

    /*
     * Android has very different sysconf tokens.
     */

    if(token == 0x27) {
        translated = _SC_PAGESIZE;
    } else if(token == 0x61) {
        translated = _SC_NPROCESSORS_ONLN;
    } else {
        /*
         * Nothing good will come from failing this call, so just panic.
         */
        panic("android_sysconf: unknown token %d\n", token);
    }

    return sysconf(translated);
}


#include "SystemAPIThunking.h"
#include "ThunkManager.h"
#include "thunking.h"

#include <cstdint>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <cmath>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <syslog.h>
#include <netdb.h>
#include <locale.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <semaphore.h>
#include <termios.h>
#include <sys/utsname.h>

#include "android_api.h"
#include "signal_compatibility.h"

using namespace std::string_view_literals;

extern "C" {
    size_t __ctype_get_mb_cur_max(void);
}

using SymbolProvidingFunction = void *(*)();


template<auto x86Function>
static void *thunkX86() {
    return ThunkManager::allocateARMToX86ThunkCall(reinterpret_cast<void *>(x86Function),
                                                   createTypedX86Thunk(x86Function));
}

template<void (*x86Function)(void)>
static void *thunkX86Raw() {
    return ThunkManager::allocateARMToX86ThunkCall(reinterpret_cast<void *>(x86Function), x86Function);
}

static void *getEnvironPointer() {
    return environ;
}

static void *getFileArrayPointer() {
    /*
     * TODO: do something with this thing.
     */
    return reinterpret_cast<void *>(UINT64_C(0xDEADDEADDEADDEAD));
}

static int ioctl_compat(int fd, unsigned long request, void *data) {
    return ioctl(fd, request, data);
}

static int open_compat(const char *name, int flags, mode_t mode) {
    return open(name, flags, mode);
}

static int fcntl_compat(int fd, int cmd, uintptr_t arg) {
    return fcntl(fd, cmd, arg);
}

static void replace_stack_chk_fail() {
    panic("__stack_chk_fail called");
}

static void *memchr_compat(const void *s, int c, size_t n) {
    return const_cast<void *>(memchr(s, c, n));
}

static char *strchr_compat(const char *s, int c) {
    return const_cast<char *>(strchr(s, c));
}

static char *strrchr_compat(const char *s, int c) {
    return const_cast<char *>(strrchr(s, c));
}

static char *strstr_compat(const char *haystack, const char *needle) {
    return const_cast<char *>(strstr(haystack, needle));
}

static wchar_t *wmemchr_compat(const wchar_t *s, wchar_t c, size_t n) {
    return const_cast<wchar_t *>(wmemchr(s, c, n));
}

static int *errno_compat() {
    return &errno;
}

static void syslogRawThunk(void) {
    panic("syslogRawThunk\n");
}

static void vasprintfRawThunk(void) {
    panic("vasprintfRawThunk\n");
}

static void vfprintfRawThunk(void) {
    panic("vfprintfRawThunk\n");
}

static void divRawThunk(void) {
    panic("divRawThunk");
}

static int pthread_attr_getstack_compat(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    printf("pthread_attr_getstack(%p, %p, %p)\n", attr, stackaddr, stacksize);

    auto result = pthread_attr_getstack(attr, stackaddr, stacksize);

    if(result == 0) {
        auto addr = *stackaddr;
        auto size = *stacksize;

        void *currentHostSP;
        __asm__ ("movq %%rsp, %0" : "=r"(currentHostSP));

        auto end = static_cast<unsigned char *>(addr) + size;

        printf("  pthread_attr_getstack query; stack range: %p<->%p, current host SP: %p\n",
               addr, end, currentHostSP);

        if(currentHostSP >= addr && currentHostSP <= end) {
            /*
             * Boehm GC queries the stack area of the current thread for the
             * value tracking purposes.
             *
             * We need to redirect it to the ARM stack.
             */

            printf("    querying the stack range of the current thread, subsituting the ARM stack\n");

            auto &context = JITThreadContext::get();

            *stackaddr = context.threadARMStack();
            *stacksize = context.threadARMStackSize();
        }

        return result;
    }

    return result;
}

static void sscanfRawThunk(void) {
    panic("sscanfRawThunk\n");
}

static void swprintfRawThunk(void) {
    panic("swprintfRawThunk\n");
}

static void vsscanfRawThunk(void) {
    panic("vsscanfRawThunk\n");
}

static void abortWrapped(void) {
    JIT::stopDebuggerIfAttached(SIGABRT);
    abort();
}

static const std::unordered_map<std::string_view, SymbolProvidingFunction> systemAPI{

    { "abort"sv, &thunkX86<abortWrapped> },
    { "accept"sv, &thunkX86<accept> },
    { "access"sv, &thunkX86<access> },
    { "acos"sv, &thunkX86<acos> },
    { "acosf"sv, &thunkX86<acosf> },
    { "__android_log_print"sv, &thunkX86Raw<android_log_print_raw_thunk> },
    { "android_set_abort_message"sv, &thunkX86<android_set_abort_message> },
    { "asin"sv, &thunkX86<asin> },
    { "asinf"sv, &thunkX86<asinf> },
    { "atan"sv, &thunkX86<atan> },
    { "atan2"sv, &thunkX86<atan2> },
    { "atan2f"sv, &thunkX86<atan2f> },
    { "atanf"sv, &thunkX86<atanf> },
    { "atoi"sv, &thunkX86<atoi> },
    { "atol"sv, &thunkX86<atol> },
    { "bind"sv, &thunkX86<bind> },
    { "bsearch"sv, &thunkX86<bsearch> },
    { "btowc"sv, &thunkX86<btowc> },
    { "calloc"sv, &thunkX86<calloc> },
    { "clock"sv, &thunkX86<clock> },
    { "clock_getres"sv, &thunkX86<clock_getres> },
    { "clock_gettime"sv, &thunkX86<clock_gettime> },
    { "close"sv, &thunkX86<close> },
    { "closedir"sv, &thunkX86<closedir> },
    { "closelog"sv, &thunkX86<closelog> },
    { "connect"sv, &thunkX86<connect> },
    { "cos"sv, &thunkX86<cos> },
    { "cosf"sv, &thunkX86<cosf> },
    { "__ctype_get_mb_cur_max"sv, &thunkX86<__ctype_get_mb_cur_max> },
    { "__cxa_atexit", &thunkX86<emulated_cxa_atexit> },
    { "__cxa_finalize", &thunkX86<emulated_cxa_finalize> },
    { "difftime", &thunkX86<difftime> },
    { "div", &thunkX86Raw<divRawThunk> },
    { "dladdr", &thunkX86<emulated_dladdr> },
    { "dlclose", &thunkX86<emulated_dlclose> },
    { "dlerror", &thunkX86<emulated_dlerror> },
    { "dl_iterate_phdr", &thunkX86<emulated_dl_iterate_phdr> },
    { "dlopen", &thunkX86<emulated_dlopen> },
    { "dlsym", &thunkX86<emulated_dlsym> },
    { "dup", &thunkX86<dup> },
    { "dup2", &thunkX86<dup2> },
    { "environ", &getEnvironPointer },
    { "__errno", &thunkX86<errno_compat> },
    { "exit", &thunkX86<exit> },
    { "exp", &thunkX86<exp> },
    { "exp2f", &thunkX86<exp2f> },
    { "expf", &thunkX86<expf> },
    { "fclose", &thunkX86<fclose> },
    { "fcntl", &thunkX86<fcntl_compat> },
    { "__FD_SET_chk", &thunkX86<android_FD_SET_chk> },
    { "fflush", &thunkX86<fflush> },
    { "fmod", &thunkX86<fmod> },
    { "fmodf", &thunkX86<fmodf> },
    { "fopen", &thunkX86<fopen> },
    { "fputc", &thunkX86<fputc> },
    { "fread", &thunkX86<fread> },
    { "free", &thunkX86<free> },
    { "freeaddrinfo", &thunkX86<freeaddrinfo> },
    { "freelocale", &thunkX86<freelocale> },
    { "fseek", &thunkX86<fseek> },
    { "fseeko", &thunkX86<fseeko> },
    /*
     * TODO: struct stat may be incompatible
     */
    { "fstat", &thunkX86<android_fstat> },
    { "ftello", &thunkX86<ftello> },
    { "ftruncate", &thunkX86<ftruncate> },
    { "fwrite", &thunkX86<fwrite> },
    { "getaddrinfo", &thunkX86<getaddrinfo> },
    { "getcwd", &thunkX86<getcwd> },
    { "getenv", &thunkX86<getenv> },
    { "gethostname", &thunkX86<gethostname> },
    { "getnameinfo", &thunkX86<getnameinfo> },
    { "getpid", &thunkX86<getpid> },
    { "getsockname", &thunkX86<getsockname> },
    { "getsockopt", &thunkX86<getsockopt> },
    { "gettimeofday", &thunkX86<gettimeofday> },
    { "gmtime", &thunkX86<gmtime> },
    { "hypot", &thunkX86<hypot> },
    { "inet_ntop", &thunkX86<inet_ntop> },
    { "inet_pton", &thunkX86<inet_pton> },
    { "ioctl", &thunkX86<ioctl_compat> },
    { "isalpha", &thunkX86<isalpha> },
    { "isatty", &thunkX86<isatty> },
    { "islower", &thunkX86<islower> },
    { "isspace", &thunkX86<isspace> },
    { "isupper", &thunkX86<isupper> },
    { "iswalpha", &thunkX86<iswalpha> },
    { "iswblank", &thunkX86<iswblank> },
    { "iswcntrl", &thunkX86<iswcntrl> },
    { "iswdigit", &thunkX86<islower> },
    { "iswlower", &thunkX86<iswlower> },
    { "iswprint", &thunkX86<iswprint> },
    { "iswpunct", &thunkX86<iswpunct> },
    { "iswspace", &thunkX86<iswspace> },
    { "iswupper", &thunkX86<iswupper> },
    { "iswxdigit", &thunkX86<iswxdigit> },
    { "isxdigit", &thunkX86<isxdigit> },
    { "ldexpf", &thunkX86<ldexpf> },
    { "listen", &thunkX86<listen> },
    { "localeconv", &thunkX86<localeconv> },
    { "localtime", &thunkX86<localtime> },
    { "log", &thunkX86<log> },
    { "log10", &thunkX86<log10> },
    { "log10f", &thunkX86<log10f> },
    { "logb", &thunkX86<logb> },
    { "logf", &thunkX86<logf> },
    { "lseek", &thunkX86<lseek> },
    /*
     * TODO: struct stat may be incompatible
     */
    { "lstat", &thunkX86<android_lstat> },
    { "malloc", &thunkX86<malloc> },
    { "mbrlen", &thunkX86<mbrlen> },
    { "mbrtowc", &thunkX86<mbrtowc> },
    { "mbsnrtowcs", &thunkX86<mbsnrtowcs> },
    { "mbsrtowcs", &thunkX86<mbsrtowcs> },
    { "mbtowc", &thunkX86<mbtowc> },
    { "memalign", &thunkX86<memalign> },
    { "memchr", &thunkX86<memchr_compat> },
    { "memcmp", &thunkX86<memcmp> },
    { "memcpy", &thunkX86<memcpy> },
    { "memmove", &thunkX86<memmove> },
    { "memset", &thunkX86<memset> },
    { "mktime", &thunkX86<mktime> },
    { "mkdir", &thunkX86<mkdir> },
    { "mmap", &thunkX86<mmap> },
    { "modf", &thunkX86<modf> },
    { "mprotect", &thunkX86<mprotect> },
    { "munmap", &thunkX86<munmap> },
    { "nanosleep", &thunkX86<nanosleep> },
    { "newlocale", &thunkX86<newlocale> },
    { "open", &thunkX86<open_compat> },
    { "opendir", &thunkX86<opendir> },
    { "openlog", &thunkX86<openlog> },
    { "pipe", &thunkX86<pipe> },
    { "poll", &thunkX86<poll> },
    { "posix_memalign", &thunkX86<posix_memalign> },
    { "pow", &thunkX86<pow> },
    { "powf", &thunkX86<powf> },
    { "pthread_atfork", &thunkX86<pthread_atfork> },
    { "pthread_attr_destroy", &thunkX86<pthread_attr_destroy> },
    { "pthread_attr_getstack", &thunkX86<pthread_attr_getstack_compat> },
    { "pthread_attr_init", &thunkX86<pthread_attr_init> },
    { "pthread_condattr_destroy", &thunkX86<pthread_condattr_destroy> },
    { "pthread_condattr_init", &thunkX86<pthread_condattr_init> },
    { "pthread_condattr_setclock", &thunkX86<pthread_condattr_setclock> },
    { "pthread_cond_broadcast", &thunkX86<pthread_cond_broadcast> },
    { "pthread_cond_destroy", &thunkX86<pthread_cond_destroy> },
    { "pthread_cond_init", &thunkX86<pthread_cond_init> },
    { "pthread_cond_signal", &thunkX86<pthread_cond_signal> },
    { "pthread_cond_timedwait", &thunkX86<pthread_cond_timedwait> },
    { "pthread_cond_wait", &thunkX86<pthread_cond_wait> },
    { "pthread_create", &thunkX86<pthread_create> },
    { "pthread_detach", &thunkX86<pthread_detach> },
    { "pthread_equal", &thunkX86<pthread_equal> },
    { "pthread_getattr_np", &thunkX86<pthread_getattr_np> },
    { "pthread_getspecific", &thunkX86<pthread_getspecific> },
    { "pthread_join", &thunkX86<pthread_join> },
    { "pthread_key_create", &thunkX86<pthread_key_create> },
    { "pthread_key_delete", &thunkX86<pthread_key_delete> },
    { "pthread_kill", &thunkX86<pthread_kill> },
    { "pthread_mutexattr_destroy", &thunkX86<pthread_mutexattr_destroy> },
    { "pthread_mutexattr_init", &thunkX86<pthread_mutexattr_init> },
    { "pthread_mutexattr_settype", &thunkX86<pthread_mutexattr_settype> },
    { "pthread_mutex_destroy", &thunkX86<pthread_mutex_destroy> },
    { "pthread_mutex_init", &thunkX86<pthread_mutex_init> },
    { "pthread_mutex_lock", &thunkX86<pthread_mutex_lock> },
    { "pthread_mutex_trylock", &thunkX86<pthread_mutex_trylock> },
    { "pthread_mutex_unlock", &thunkX86<pthread_mutex_unlock> },
    { "pthread_once", &thunkX86<pthread_once> },
    { "pthread_rwlock_destroy", &thunkX86<pthread_rwlock_destroy> },
    { "pthread_rwlock_init", &thunkX86<pthread_rwlock_init> },
    { "pthread_rwlock_rdlock", &thunkX86<pthread_rwlock_rdlock> },
    { "pthread_rwlock_unlock", &thunkX86<pthread_rwlock_unlock> },
    { "pthread_rwlock_wrlock", &thunkX86<pthread_rwlock_wrlock> },
    { "pthread_self", &thunkX86<pthread_self> },
    { "pthread_setname_np", &thunkX86<pthread_setname_np> },
    { "pthread_setspecific", &thunkX86<pthread_setspecific> },
    { "pthread_sigmask", &thunkX86<pthread_sigmask> },
    { "read", &thunkX86<read> },
    { "readdir", &thunkX86<readdir> },
    { "readlink", &thunkX86<readlink> },
    { "realloc", &thunkX86<realloc> },
    { "recvfrom", &thunkX86<recvfrom> },
    { "recvmsg", &thunkX86<recvmsg> },
    { "rename", &thunkX86<rename> },
    { "rmdir", &thunkX86<rmdir> },
    { "scalbn", &thunkX86<scalbn> },
    { "sched_yield", &thunkX86<sched_yield> },
    { "select", &thunkX86<select> },
    { "sem_getvalue", &thunkX86<sem_getvalue> },
    { "sem_init", &thunkX86<sem_init> },
    { "sem_post", &thunkX86<sem_post> },
    { "sem_timedwait", &thunkX86<sem_timedwait> },
    { "sem_wait", &thunkX86<sem_wait> },
    { "send", &thunkX86<send> },
    { "sendmsg", &thunkX86<sendmsg> },
    { "setenv", &thunkX86<setenv> },
    //{ "setjmp", &thunkX86Raw<arm_setjmp> }, - armlib
    { "setlocale", &thunkX86<setlocale> },
    { "setsockopt", &thunkX86<setsockopt> },
    { "__sF", &getFileArrayPointer },
    { "shutdown", &thunkX86<shutdown> },
    { "sigaction", &thunkX86<sigaction_compat> },
    { "sigaddset", &thunkX86<sigaddset> },
    { "sigdelset", &thunkX86<sigdelset> },
    { "sigemptyset", &thunkX86<sigemptyset> },
    { "sigfillset", &thunkX86<sigfillset> },
    { "sigprocmask", &thunkX86<sigprocmask> },
    { "signal", &thunkX86<signal> },
    { "sigsuspend", &thunkX86<sigsuspend> },
    { "sin", &thunkX86<sin> },
    { "sinf", &thunkX86<sinf> },
    //{ "snprintf", &thunkX86Raw<snprintfRawThunk> }, - armlib
    { "socket", &thunkX86<socket> },
    //{ "sprintf", &thunkX86Raw<sprintfRawThunk> }, - armlib
    { "sqrt", &thunkX86<sqrt> },
    { "sqrtf", &thunkX86<sqrtf> },
    { "sscanf", &thunkX86<sscanfRawThunk> },
    { "__stack_chk_fail", &thunkX86<replace_stack_chk_fail> },
    { "stat", &thunkX86<android_stat> },
    { "strchr", &thunkX86<strchr_compat> },
    { "strcmp", &thunkX86<strcmp> },
    { "strcoll", &thunkX86<strcoll> },
    { "strcpy", &thunkX86<strcpy> },
    { "strerror_r", &thunkX86<strerror_r> },
    { "strftime", &thunkX86<strftime> },
    { "strlcpy", &thunkX86<strlcpy> },
    { "strlen", &thunkX86<strlen> },
    { "strncmp", &thunkX86<strncmp> },
    { "strncpy", &thunkX86<strncpy> },
    { "strrchr", &thunkX86<strrchr_compat> },
    { "strstr", &thunkX86<strstr_compat> },
    { "strtod", &thunkX86<strtod> },
    { "strtof", &thunkX86<strtof> },
    { "strtol", &thunkX86<strtol> },
    { "strtold", &thunkX86<strtold> },
    { "strtold_l", &thunkX86<strtold_l> },
    { "strtoll", &thunkX86<strtoll> },
    { "strtoll_l", &thunkX86<strtoll_l> },
    { "strtoul", &thunkX86<strtoul> },
    { "strtoull", &thunkX86<strtoull> },
    { "strtoull_l", &thunkX86<strtoull_l> },
    { "strxfrm", &thunkX86<strxfrm> },
    { "swprintf", &thunkX86<swprintfRawThunk> },
    { "sysconf", &thunkX86<sysconf> },
    { "syslog", &thunkX86<syslogRawThunk> },
    { "__system_property_get", &thunkX86<android_system_property_get> },
    { "tan", &thunkX86<tan> },
    { "tanf", &thunkX86<tanf> },
    { "tcflush", &thunkX86<tcflush> },
    { "tcgetattr", &thunkX86<tcgetattr> },
    { "tcsetattr", &thunkX86<tcsetattr> },
    { "time", &thunkX86<time> },
    { "tolower", &thunkX86<tolower> },
    { "toupper", &thunkX86<toupper> },
    { "towlower", &thunkX86<towlower> },
    { "towupper", &thunkX86<towupper> },
    { "uname", &thunkX86<uname> },
    { "unlink", &thunkX86<unlink> },
    { "unsetenv", &thunkX86<unsetenv> },
    { "uselocale", &thunkX86<uselocale> },
    { "usleep", &thunkX86<usleep> },
    { "vasprintf", &thunkX86Raw<vasprintfRawThunk> },
    { "vfprintf", &thunkX86Raw<vfprintfRawThunk> },
    //{ "vsnprintf", &thunkX86Raw<vsnprintfRawThunk> }, - armlib
    { "vsscanf", &thunkX86Raw<vsscanfRawThunk> },
    { "wcrtomb", &thunkX86<wcrtomb> },
    { "wcscoll", &thunkX86<wcscoll> },
    { "wcslen", &thunkX86<wcslen> },
    { "wcsnrtombs", &thunkX86<wcsnrtombs> },
    { "wcstod", &thunkX86<wcstod> },
    { "wcstof", &thunkX86<wcstof> },
    { "wcstol", &thunkX86<wcstol> },
    { "wcstold", &thunkX86<wcstold> },
    { "wcstoll", &thunkX86<wcstoll> },
    { "wcstoul", &thunkX86<wcstoul> },
    { "wcstoull", &thunkX86<wcstoull> },
    { "wcsxfrm", &thunkX86<wcsxfrm> },
    { "wctob", &thunkX86<wctob> },
    { "wmemchr", &thunkX86<wmemchr_compat> },
    { "wmemcmp", &thunkX86<wmemcmp> },
    { "wmemcpy", &thunkX86<wmemcpy> },
    { "wmemmove", &thunkX86<wmemmove> },
    { "wmemset", &thunkX86<wmemset> },
    { "write", &thunkX86<write> },
};

void *resolveUndefinedARMSymbol(const std::string_view &name) {

    auto it = systemAPI.find(name);
    if(it == systemAPI.end()) {
        throw std::runtime_error("System API symbol is not implemented: " + std::string(name));
    }

    return it->second();

#if 0
    printf("requested to resolve an undefined (external) symbol %s\n", name);

    return ThunkManager::allocateARMToX86ThunkCall(name);
#endif
}

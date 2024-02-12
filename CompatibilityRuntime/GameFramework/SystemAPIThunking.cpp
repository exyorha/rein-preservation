#include "SystemAPIThunking.h"
#include <Translator/ThunkManager.h>
#include <Translator/thunking.h>
#include <Bionic/BionicCallouts.h>
#include <Bionic/BionicThreading.h>

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
#include "grpc_csharp_ext.h"
#include "grpc_special_thunks.h"
#include "grpc_channel_redirection.h"

using namespace std::string_view_literals;

using SymbolProvidingFunction = void *(*)();


template<auto x86Function>
static void *thunkX86() {
    return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(x86Function),
                                                   createTypedX86Thunk(x86Function));
}

template<void (*x86Function)(void)>
static void *thunkX86Raw() {
    return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(x86Function), x86Function);
}

int plat_openat(int fd, const char* path, int oflag, int mode) {
    auto result = openat(fd, path, oflag, mode);
    if(result < 0) {
        bionic_set_errno(errno);
    }

    return result;
}

int plat_gettimeofday(struct timeval *tp, void *tzp) {
    auto result = gettimeofday(tp, tzp);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

void *plat_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
    auto result = mmap(addr, len, prot, flags, fildes, off);
    if(result == MAP_FAILED)
        bionic_set_errno(errno);

    return result;
}

int plat_mprotect(void *addr, size_t len, int prot) {
    auto result = mprotect(addr, len, prot);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_munmap(void *addr, size_t len) {
    auto result = munmap(addr, len);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_fstat(int fd, struct android_stat *statbuf) {
    auto result = android_fstat(fd, statbuf);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_fstatat(int fd, const char *path, struct android_stat *statbuf, int flag) {
    auto result = android_fstatat(fd, path, statbuf, flag);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_dup(int fd) {
    auto result = dup(fd);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_close(int fd) {
    auto result = close(fd);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_clock_gettime(clockid_t clockid, struct timespec *tp) {
    auto result = clock_gettime(clockid, tp);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_clock_getres(clockid_t clockid, struct timespec *tp) {
    auto result = clock_getres(clockid, tp);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

ssize_t plat_read(int fd, void *dest, size_t size) {
    auto result = read(fd, dest, size);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

ssize_t plat_write(int fd, const void *dest, size_t size) {
    auto result = write(fd, dest, size);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

ssize_t plat_readlinkat(int dirfd, const char *pathname,
                        char *buf, size_t bufsiz) {

    auto result = readlinkat(dirfd, pathname, buf, bufsiz);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_faccessat(int fd, const char *path, int amode, int flag) {
    auto result = faccessat(fd, path, amode, flag);

    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_uname(bionic_utsname *name) {
    utsname host;
    auto result = uname(&host);
    if(result < 0)
        bionic_set_errno(errno);
    else {
        strncpy(name->sysname, host.sysname, sizeof(name->sysname));
        strncpy(name->nodename, host.nodename, sizeof(name->nodename));
        strncpy(name->release, host.release, sizeof(name->release));
        strncpy(name->version, host.version, sizeof(name->version));
        strncpy(name->machine, host.machine, sizeof(name->machine));
        strncpy(name->release, host.release, sizeof(name->release));
    }

    return result;
}

int plat_renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
    auto result = renameat(olddirfd, oldpath, newdirfd, newpath);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_unlinkat(int dirfd, const char *pathname, int flags) {
    auto result = unlinkat(dirfd, pathname, flags);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_system_property_get(const char *name, char *value) {
    auto result = android_system_property_get(name, value);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_mkdirat(int dirfd, const char *pathname, bionic_mode_t mode) {
    auto result = mkdirat(dirfd, pathname, mode);
    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

off_t plat_lseek(int fildes, off_t offset, int whence) {
    auto result = lseek(fildes, offset, whence);
    if(result < 0)
        bionic_set_errno(errno);

    return result;

}

int plat_nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
    auto result = nanosleep(rqtp, rmtp);

    if(result < 0)
        bionic_set_errno(errno);

    return result;
}

int plat_write_log(int priority, const char* tag, const char* msg) {
    printf("Android log: prio %d, tag '%s': %s\n", priority, tag, msg);

    return 0;
}

static const std::unordered_map<std::string_view, SymbolProvidingFunction> systemAPI{
/*
 * NEW SET FOR BIONIC. Syscalls
 */
    { "__openat", &thunkX86<plat_openat> },
    { "gettimeofday", &thunkX86<plat_gettimeofday> },
    { "mmap", &thunkX86<plat_mmap> },
    { "mprotect", &thunkX86<plat_mprotect> },
    { "munmap", &thunkX86<plat_munmap> },
    { "fstat", &thunkX86<plat_fstat> },
    { "fstatat", &thunkX86<plat_fstatat> },
    { "dup", &thunkX86<plat_dup> },
    { "___close", &thunkX86<plat_close> },
    { "clock_gettime", &thunkX86<plat_clock_gettime> },
    { "read", &thunkX86<plat_read> },
    { "write", &thunkX86<plat_write> },
    { "___getpid", &thunkX86<getpid> },
    { "readlinkat", &thunkX86<plat_readlinkat> },
    { "renameat", &thunkX86<plat_renameat> },
    { "unlinkat", &thunkX86<plat_unlinkat> },
    { "___faccessat", &thunkX86<plat_faccessat> },
    { "uname", &thunkX86<plat_uname> },
    { "clock_getres", &thunkX86<plat_clock_getres> },
    { "sched_yield", &thunkX86<sched_yield> },
    { "mkdirat", &thunkX86<plat_mkdirat> },
    { "lseek", &thunkX86<plat_lseek> },
    { "nanosleep", &thunkX86<plat_nanosleep> },

    /*
     * Android-specific API that we delegate out to x86 code.
     */
    { "__system_property_get", &thunkX86<plat_system_property_get> },
    { "__compatibility_runtime_write_log", &thunkX86<plat_write_log> },

    /*
     * pthread API, the portion that we implement in x86 code.
     */
    { "pthread_mutex_init", &thunkX86<plat_pthread_mutex_init> },
    { "pthread_mutex_destroy", &thunkX86<plat_pthread_mutex_destroy> },
    { "pthread_mutex_lock", &thunkX86<plat_pthread_mutex_lock> },
    { "pthread_mutex_timedlock", &thunkX86<plat_pthread_mutex_timedlock> },
    { "pthread_mutex_trylock", &thunkX86<plat_pthread_mutex_trylock> },
    { "pthread_mutex_unlock", &thunkX86<plat_pthread_mutex_unlock> },

    { "pthread_cond_broadcast", &thunkX86<plat_pthread_cond_broadcast> },
    { "pthread_cond_destroy", &thunkX86<plat_pthread_cond_destroy> },
    { "pthread_cond_init", &thunkX86<plat_pthread_cond_init> },
    { "pthread_cond_signal", &thunkX86<plat_pthread_cond_signal> },
    { "pthread_cond_timedwait", &thunkX86<plat_pthread_cond_timedwait> },
    { "pthread_cond_wait", &thunkX86<plat_pthread_cond_wait> },

    { "pthread_rwlock_destroy", &thunkX86<plat_pthread_rwlock_destroy> },
    { "pthread_rwlock_init", &thunkX86<plat_pthread_rwlock_init> },
    { "pthread_rwlock_rdlock", &thunkX86<plat_pthread_rwlock_rdlock> },
    { "pthread_rwlock_timedrdlock", &thunkX86<plat_pthread_rwlock_timedrdlock> },
    { "pthread_rwlock_timedwrlock", &thunkX86<plat_pthread_rwlock_timedwrlock> },
    { "pthread_rwlock_tryrdlock", &thunkX86<plat_pthread_rwlock_tryrdlock> },
    { "pthread_rwlock_trywrlock", &thunkX86<plat_pthread_rwlock_trywrlock> },
    { "pthread_rwlock_unlock", &thunkX86<plat_pthread_rwlock_unlock> },
    { "pthread_rwlock_wrlock", &thunkX86<plat_pthread_rwlock_wrlock> },

    { "pthread_create", &thunkX86<plat_pthread_create> },
    { "pthread_getattr_np", &thunkX86<plat_pthread_getattr_np> },
    { "pthread_detach", &thunkX86<plat_pthread_detach> },
    { "pthread_join", &thunkX86<plat_pthread_join> },
    { "pthread_exit", &thunkX86<plat_pthread_exit> },
    { "pthread_setname_np", &thunkX86<plat_pthread_setname_np> },
    { "pthread_kill", &thunkX86<plat_pthread_kill> },

    /*
     * Loader API
     */
    { "dladdr", &thunkX86<emulated_dladdr> },
    { "dlclose", &thunkX86<emulated_dlclose> },
    { "dlerror", &thunkX86<emulated_dlerror> },
    { "dl_iterate_phdr", &thunkX86<emulated_dl_iterate_phdr> },
    { "dlopen", &thunkX86<emulated_dlopen> },
    { "dlsym", &thunkX86<emulated_dlsym> },

/*
 * END OF THE NEW SET
 */


    { "grpcsharp_metadata_array_destroy_full", &thunkX86<grpcsharp_metadata_array_destroy_full> },
    { "grpcsharp_metadata_array_create", &thunkX86<grpcsharp_metadata_array_create> },
    { "grpcsharp_metadata_array_add", &thunkX86<grpcsharp_metadata_array_add> },
    { "grpcsharp_metadata_array_count", &thunkX86<grpcsharp_metadata_array_count> },
    { "grpcsharp_metadata_array_get_key", &thunkX86<grpcsharp_metadata_array_get_key> },
    { "grpcsharp_metadata_array_get_value", &thunkX86<grpcsharp_metadata_array_get_value> },
    { "grpcsharp_batch_context_create", &thunkX86<grpcsharp_batch_context_create> },
    { "grpcsharp_batch_context_reset", &thunkX86<grpcsharp_batch_context_reset> },
    { "grpcsharp_batch_context_destroy", &thunkX86<grpcsharp_batch_context_destroy> },
    { "grpcsharp_batch_context_recv_initial_metadata", &thunkX86<grpcsharp_batch_context_recv_initial_metadata> },
    { "grpcsharp_batch_context_recv_message_length", &thunkX86<grpcsharp_batch_context_recv_message_length> },
    { "grpcsharp_batch_context_recv_message_next_slice_peek", &thunkX86<grpcsharp_batch_context_recv_message_next_slice_peek> },
    { "grpcsharp_batch_context_recv_status_on_client_status", &thunkX86<grpcsharp_batch_context_recv_status_on_client_status> },
    { "grpcsharp_batch_context_recv_status_on_client_details", &thunkX86<grpcsharp_batch_context_recv_status_on_client_details> },
    { "grpcsharp_batch_context_recv_status_on_client_error_string", &thunkX86<grpcsharp_batch_context_recv_status_on_client_error_string> },
    { "grpcsharp_batch_context_recv_status_on_client_trailing_metadata", &thunkX86<grpcsharp_batch_context_recv_status_on_client_trailing_metadata> },
    { "grpcsharp_batch_context_recv_close_on_server_cancelled", &thunkX86<grpcsharp_batch_context_recv_close_on_server_cancelled> },
    { "grpcsharp_request_call_context_create", &thunkX86<grpcsharp_request_call_context_create> },
    { "grpcsharp_request_call_context_reset", &thunkX86<grpcsharp_request_call_context_reset> },
    { "grpcsharp_request_call_context_destroy", &thunkX86<grpcsharp_request_call_context_destroy> },
    { "grpcsharp_request_call_context_call", &thunkX86<grpcsharp_request_call_context_call> },
    { "grpcsharp_request_call_context_method", &thunkX86<grpcsharp_request_call_context_method> },
    { "grpcsharp_request_call_context_host", &thunkX86<grpcsharp_request_call_context_host> },
    { "grpcsharp_request_call_context_deadline", &thunkX86<grpcsharp_request_call_context_deadline> },
    { "grpcsharp_request_call_context_request_metadata", &thunkX86<grpcsharp_request_call_context_request_metadata> },
    { "grpcsharp_init", &thunkX86<grpcsharp_init> },
    { "grpcsharp_shutdown", &thunkX86<grpcsharp_shutdown> },
    { "grpcsharp_completion_queue_create_async", &thunkX86<grpcsharp_completion_queue_create_async> },
    { "grpcsharp_completion_queue_create_sync", &thunkX86<grpcsharp_completion_queue_create_sync> },
    { "grpcsharp_completion_queue_shutdown", &thunkX86<grpcsharp_completion_queue_shutdown> },
    { "grpcsharp_completion_queue_destroy", &thunkX86<grpcsharp_completion_queue_destroy> },
    { "grpcsharp_completion_queue_next", &thunkX86<grpcsharp_completion_queue_next> },
    { "grpcsharp_completion_queue_pluck", &thunkX86<grpcsharp_completion_queue_pluck> },
    { "grpcsharp_insecure_channel_create", &thunkX86<grpcsharp_insecure_channel_create_with_redirection> },
    { "grpcsharp_channel_destroy", &thunkX86<grpcsharp_channel_destroy> },
    { "grpcsharp_channel_check_connectivity_state", &thunkX86<grpcsharp_channel_check_connectivity_state> },
    { "grpcsharp_channel_watch_connectivity_state", &thunkX86Raw<grpcsharp_channel_watch_connectivity_state_raw_thunk> },
    { "grpcsharp_channel_get_target", &thunkX86<grpcsharp_channel_get_target> },
    { "grpcsharp_channel_args_create", &thunkX86<grpcsharp_channel_args_create> },
    { "grpcsharp_channel_args_set_string", &thunkX86<grpcsharp_channel_args_set_string> },
    { "grpcsharp_channel_args_set_integer", &thunkX86<grpcsharp_channel_args_set_integer> },
    { "grpcsharp_channel_args_destroy", &thunkX86<grpcsharp_channel_args_destroy> },
    { "grpcsharp_channel_create_call", &thunkX86Raw<grpcsharp_channel_create_call_raw_thunk> },
    { "grpcsharp_call_cancel", &thunkX86<grpcsharp_call_cancel> },
    { "grpcsharp_call_cancel_with_status", &thunkX86<grpcsharp_call_cancel_with_status> },
    { "grpcsharp_call_get_peer", &thunkX86<grpcsharp_call_get_peer> },
    { "grpcsharp_call_destroy", &thunkX86<grpcsharp_call_destroy> },
    { "grpcsharp_call_start_unary", &thunkX86<grpcsharp_call_start_unary> },
    { "grpcsharp_test_call_start_unary_echo", &thunkX86<grpcsharp_test_call_start_unary_echo> },
    { "grpcsharp_call_start_client_streaming", &thunkX86<grpcsharp_call_start_client_streaming> },
    { "grpcsharp_call_start_server_streaming", &thunkX86<grpcsharp_call_start_server_streaming> },
    { "grpcsharp_call_start_duplex_streaming", &thunkX86<grpcsharp_call_start_duplex_streaming> },
    { "grpcsharp_call_recv_initial_metadata", &thunkX86<grpcsharp_call_recv_initial_metadata> },
    { "grpcsharp_call_send_message", &thunkX86<grpcsharp_call_send_message> },
    { "grpcsharp_call_send_close_from_client", &thunkX86<grpcsharp_call_send_close_from_client> },
    { "grpcsharp_call_send_status_from_server", &thunkX86<grpcsharp_call_send_status_from_server> },
    { "grpcsharp_call_recv_message", &thunkX86<grpcsharp_call_recv_message> },
    { "grpcsharp_call_start_serverside", &thunkX86<grpcsharp_call_start_serverside> },
    { "grpcsharp_call_send_initial_metadata", &thunkX86<grpcsharp_call_send_initial_metadata> },
    { "grpcsharp_call_set_credentials", &thunkX86<grpcsharp_call_set_credentials> },
    { "gprsharp_now", &thunkX86<gprsharp_now> },
    { "gprsharp_inf_future", &thunkX86<gprsharp_inf_future> },
    { "gprsharp_inf_past", &thunkX86<gprsharp_inf_past> },
    { "gprsharp_convert_clock_type", &thunkX86Raw<gprsharp_convert_clock_type_raw_thunk> },
    { "gprsharp_sizeof_timespec", &thunkX86<gprsharp_sizeof_timespec> },
    { "gprsharp_free", &thunkX86<gprsharp_free> },
    { "grpcsharp_server_create", &thunkX86<grpcsharp_server_create> },
    { "grpcsharp_server_register_completion_queue", &thunkX86<grpcsharp_server_register_completion_queue> },
    { "grpcsharp_server_add_insecure_http2_port", &thunkX86<grpcsharp_server_add_insecure_http2_port> },
    { "grpcsharp_server_add_secure_http2_port", &thunkX86<grpcsharp_server_add_secure_http2_port> },
    { "grpcsharp_server_start", &thunkX86<grpcsharp_server_start> },
    { "grpcsharp_server_shutdown_and_notify_callback", &thunkX86<grpcsharp_server_shutdown_and_notify_callback> },
    { "grpcsharp_server_cancel_all_calls", &thunkX86<grpcsharp_server_cancel_all_calls> },
    { "grpcsharp_server_destroy", &thunkX86<grpcsharp_server_destroy> },
    { "grpcsharp_server_request_call", &thunkX86<grpcsharp_server_request_call> },
    { "grpcsharp_native_callback_dispatcher_init", &thunkX86<grpcsharp_native_callback_dispatcher_init> },
    { "grpcsharp_override_default_ssl_roots", &thunkX86<grpcsharp_override_default_ssl_roots> },
    { "grpcsharp_ssl_credentials_create", &thunkX86<grpcsharp_ssl_credentials_create> },
    { "grpcsharp_channel_credentials_release", &thunkX86<grpcsharp_channel_credentials_release> },
    { "grpcsharp_call_credentials_release", &thunkX86<grpcsharp_call_credentials_release> },
    { "grpcsharp_secure_channel_create", &thunkX86<grpcsharp_secure_channel_create_with_redirection> },
    { "grpcsharp_ssl_server_credentials_create", &thunkX86<grpcsharp_ssl_server_credentials_create> },
    { "grpcsharp_server_credentials_release", &thunkX86<grpcsharp_server_credentials_release> },
    { "grpcsharp_composite_channel_credentials_create", &thunkX86<grpcsharp_composite_channel_credentials_create> },
    { "grpcsharp_composite_call_credentials_create", &thunkX86<grpcsharp_composite_call_credentials_create> },
    { "grpcsharp_metadata_credentials_notify_from_plugin", &thunkX86<grpcsharp_metadata_credentials_notify_from_plugin> },
    { "grpcsharp_metadata_credentials_create_from_plugin", &thunkX86<grpcsharp_metadata_credentials_create_from_plugin> },
    { "grpcsharp_call_auth_context", &thunkX86<grpcsharp_call_auth_context> },
    { "grpcsharp_auth_context_peer_identity_property_name", &thunkX86<grpcsharp_auth_context_peer_identity_property_name> },
    { "grpcsharp_auth_context_property_iterator", &thunkX86Raw<grpcsharp_auth_context_property_iterator_raw_thunk> },
    { "grpcsharp_auth_property_iterator_next", &thunkX86<grpcsharp_auth_property_iterator_next> },
    { "grpcsharp_auth_context_release", &thunkX86<grpcsharp_auth_context_release> },
    { "grpcsharp_redirect_log", &thunkX86<grpcsharp_redirect_log> },
    { "grpcsharp_slice_buffer_create", &thunkX86<grpcsharp_slice_buffer_create> },
    { "grpcsharp_slice_buffer_reset_and_unref", &thunkX86<grpcsharp_slice_buffer_reset_and_unref> },
    { "grpcsharp_slice_buffer_destroy", &thunkX86<grpcsharp_slice_buffer_destroy> },
    { "grpcsharp_slice_buffer_slice_count", &thunkX86<grpcsharp_slice_buffer_slice_count> },
    { "grpcsharp_slice_buffer_slice_peek", &thunkX86<grpcsharp_slice_buffer_slice_peek> },
    { "grpcsharp_slice_buffer_adjust_tail_space", &thunkX86<grpcsharp_slice_buffer_adjust_tail_space> },
    { "grpcsharp_version_string", &thunkX86<grpcsharp_version_string> },
    { "grpcsharp_test_callback", &thunkX86<grpcsharp_test_callback> },
    { "grpcsharp_test_nop", &thunkX86<grpcsharp_test_nop> },
    { "grpcsharp_sizeof_grpc_event", &thunkX86<grpcsharp_sizeof_grpc_event> },
    { "grpcsharp_test_override_method", &thunkX86<grpcsharp_test_override_method> },
};

struct dirent dir;

static void stubCall(void) {
    const char *name = static_cast<const char *>(thunkUtilitySlot);

    panic("Unimplemented system API function called: '%s'\n", name);
}

void *resolveUndefinedARMSymbol(const std::string_view &name) {

    auto it = systemAPI.find(name);
    if(it == systemAPI.end()) {
        fprintf(stderr, "System API not implemented: '%.*s', the call will fail at runtime\n", static_cast<int>(name.size()), name.data());

        auto nameDup = strdup(std::string(name).c_str());
        return GlobalContext::get().thunkManager().allocateARMToX86ThunkCall(reinterpret_cast<void *>(nameDup), stubCall);

//        throw std::runtime_error("System API symbol is not implemented: " + std::string(name));
    }

    return it->second();
}

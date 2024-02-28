cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD_LIBRARIES "")
set(CMAKE_CXX_STANDARD_LIBRARIES "")

project(CompatibilityRuntime-bionic)

enable_language(ASM)

add_library(compatibility-runtime-platform-stubs SHARED
    compatibility-runtime-platform-stubs/compatibility-runtime-platform-stubs.S
)
target_include_directories(compatibility-runtime-platform-stubs PUBLIC compatibility-runtime-platform-stubs)
target_link_options(compatibility-runtime-platform-stubs PUBLIC -nostdlib -nostartfiles)
set_target_properties(compatibility-runtime-platform-stubs PROPERTIES
    OUTPUT_NAME "compatibility-runtime"
    PREFIX ""
    LINKER_LANGUAGE C
    LINK_LIBRARIES ""
)

# We're doing some special things here.

set(libc_sources
# Make sure that this comes first
    arch-common/bionic/crtbegin_so.c
    bionic/libc_init_dynamic.cpp

# libc_common_src_files

# Define the common source files for all the libc instances
# =========================================================
    bionic/bindresvport.c
    bionic/ether_aton.c
    bionic/ether_ntoa.c
    bionic/fts.c
#    bionic/getpriority.c
    bionic/if_indextoname.c
    bionic/if_nametoindex.c
#    bionic/initgroups.c
    bionic/ioctl.c
    bionic/isatty.c
    bionic/memmem.c
    bionic/pututline.c
    bionic/sched_cpualloc.c
    bionic/sched_cpucount.c
    bionic/sigblock.c
    bionic/siginterrupt.c
    bionic/sigsetmask.c
    #bionic/system_properties_compat.c
    stdio/findfp.c
    stdio/fread.c
    stdio/snprintf.c
    stdio/sprintf.c
    stdio/stdio.c
    stdio/stdio_ext.cpp
    stdlib/atexit.c
    stdlib/exit.c

# Fortify implementations of libc functions.
    bionic/__FD_chk.cpp
    bionic/__fgets_chk.cpp
    bionic/__memchr_chk.cpp
    bionic/__memmove_chk.cpp
    bionic/__memrchr_chk.cpp
    bionic/__poll_chk.cpp
    #bionic/__pread64_chk.cpp
    #bionic/__pread_chk.cpp
    bionic/__read_chk.cpp
    bionic/__readlink_chk.cpp
    #bionic/__readlinkat_chk.cpp
    bionic/__recvfrom_chk.cpp
    bionic/__stpcpy_chk.cpp
    bionic/__stpncpy_chk.cpp
    bionic/__strchr_chk.cpp
    bionic/__strlcat_chk.cpp
    bionic/__strlcpy_chk.cpp
    bionic/__strlen_chk.cpp
    bionic/__strncat_chk.cpp
    bionic/__strncpy_chk.cpp
    bionic/__strrchr_chk.cpp
    #bionic/__umask_chk.cpp
    bionic/__vsnprintf_chk.cpp
    bionic/__vsprintf_chk.cpp

# libc_bionic_src_files
    bionic/abort.cpp
    bionic/accept.cpp
    bionic/accept4.cpp
    #bionic/access.cpp
    bionic/assert.cpp
    bionic/atof.cpp
    bionic/bionic_systrace.cpp
    bionic/bionic_time_conversions.cpp
    #bionic/brk.cpp
    bionic/c16rtomb.cpp
    bionic/c32rtomb.cpp
    #bionic/chmod.cpp
    #bionic/chown.cpp
    bionic/clearenv.cpp
    bionic/clock.cpp
    bionic/clock_getcpuclockid.cpp
    #bionic/clock_nanosleep.cpp
    #bionic/clone.cpp
    bionic/close.cpp
    bionic/__cmsg_nxthdr.cpp
    bionic/connect.cpp
    bionic/ctype.cpp
    bionic/dirent.cpp
    bionic/dup2.cpp
    #bionic/epoll_create.cpp
    #bionic/epoll_pwait.cpp
    #bionic/epoll_wait.cpp
    bionic/__errno.cpp
    bionic/error.cpp
    bionic/eventfd_read.cpp
    bionic/eventfd_write.cpp
    #bionic/faccessat.cpp
    #bionic/fchmod.cpp
    #bionic/fchmodat.cpp
    bionic/ffs.cpp
    #bionic/fgetxattr.cpp
    bionic/flockfile.cpp
    bionic/fpclassify.cpp
    #bionic/fsetxattr.cpp
    bionic/ftruncate.cpp
    #bionic/futimens.cpp
    bionic/getcwd.cpp
    #bionic/gethostname.cpp
    #bionic/getpgrp.cpp
    #bionic/getpid.cpp
    #bionic/gettid.cpp
    bionic/__gnu_basename.cpp
    #bionic/inotify_init.cpp
    #bionic/lchown.cpp
    bionic/lfs64_support.cpp
    bionic/__libc_current_sigrtmax.cpp
    bionic/__libc_current_sigrtmin.cpp
    bionic/libc_init_common.cpp
    bionic/libc_logging.cpp
    bionic/libgen.cpp
    #bionic/link.cpp
    bionic/locale.cpp
    #bionic/lstat.cpp
    bionic/malloc_info.cpp
    bionic/mbrtoc16.cpp
    bionic/mbrtoc32.cpp
    bionic/mbstate.cpp
    bionic/mempcpy.cpp
    #bionic/mkdir.cpp
    #bionic/mkfifo.cpp
    #bionic/mknod.cpp
    bionic/mntent.cpp
    bionic/NetdClientDispatch.cpp
    #bionic/open.cpp
    #bionic/pathconf.cpp
    #bionic/pause.cpp
    bionic/pipe.cpp
    bionic/poll.cpp
    #bionic/posix_fadvise.cpp
    #bionic/posix_fallocate.cpp
    #bionic/posix_madvise.cpp
    #bionic/posix_timers.cpp
    #bionic/ptrace.cpp
    #bionic/pty.cpp
    bionic/raise.cpp
    bionic/rand.cpp
    #bionic/readlink.cpp
    #bionic/reboot.cpp
    bionic/recv.cpp
    #bionic/rename.cpp
    #bionic/rmdir.cpp
    bionic/scandir.cpp
    #bionic/sched_getaffinity.cpp
    #bionic/sched_getcpu.cpp
    #bionic/semaphore.cpp
    bionic/send.cpp
    #bionic/setegid.cpp
    bionic/__set_errno.cpp
    #bionic/seteuid.cpp
    #bionic/setpgrp.cpp
    bionic/sigaction.cpp
    bionic/sigaddset.cpp
    bionic/sigdelset.cpp
    bionic/sigemptyset.cpp
    bionic/sigfillset.cpp
    bionic/sigismember.cpp
    bionic/signal.cpp
    #bionic/signalfd.cpp
    #bionic/sigpending.cpp
    bionic/sigprocmask.cpp
    #bionic/sigqueue.cpp
    bionic/sigsuspend.cpp
    #bionic/sigtimedwait.cpp
    #bionic/sigwait.cpp
    #bionic/sigwaitinfo.cpp
    bionic/socket.cpp
    #bionic/stat.cpp
    #bionic/statvfs.cpp
    bionic/strerror.cpp
    bionic/strerror_r.cpp
    bionic/strsignal.cpp
    bionic/strtold.cpp
    bionic/stubs.cpp
    #bionic/symlink.cpp
    bionic/sysinfo.cpp
    bionic/syslog.cpp
    bionic/sys_siglist.c
    bionic/sys_signame.c
    #bionic/system_properties.cpp
    bionic/tdestroy.cpp
    bionic/termios.cpp
    bionic/thread_private.cpp
    bionic/tmpfile.cpp
    #bionic/umount.cpp
    #bionic/unlink.cpp
    #bionic/utimes.cpp
    #bionic/wait.cpp
    bionic/wchar.cpp
    bionic/wctype.cpp
    bionic/wmempcpy.cpp

# libc_stack_protector
    bionic/__stack_chk_fail.cpp

# libc_malloc_src
    bionic/dlmalloc.c

    bionic/malloc_debug_common.cpp

# libc_common_src_files_arm64
    bionic/strrchr.cpp
# libc_common_src_files_arm64
    bionic/__memset_chk.cpp
    bionic/__strcpy_chk.cpp
    bionic/__strcat_chk.cpp
    bionic/strrchr.cpp
# libc_bionic_src_files_arm64
#    arch-arm64/bionic/__bionic_clone.S
#    arch-arm64/bionic/_exit_with_stack_teardown.S
#    arch-arm64/bionic/__rt_sigreturn.S
    arch-arm64/bionic/setjmp.S
    arch-arm64/bionic/__set_tls.c
#    arch-arm64/bionic/syscall.S
#    arch-arm64/bionic/vfork.S

    arch-arm64/generic/bionic/memchr.S
    arch-arm64/generic/bionic/memcmp.S
    arch-arm64/generic/bionic/memmove.S
    arch-arm64/generic/bionic/memset.S
    arch-arm64/generic/bionic/stpcpy.S
    arch-arm64/generic/bionic/strchr.S
    arch-arm64/generic/bionic/strcmp.S
    arch-arm64/generic/bionic/strcpy.S
    arch-arm64/generic/bionic/strlen.S
    arch-arm64/generic/bionic/strncmp.S
    arch-arm64/generic/bionic/strnlen.S
    arch-arm64/generic/bionic/memcpy.S
    arch-arm64/generic/bionic/wmemmove.S

#libc_pthread_src_files
    bionic/pthread_atfork.cpp
    bionic/pthread_attr.cpp
    bionic/pthread_cond.cpp
    #bionic/pthread_create.cpp
    #bionic/pthread_detach.cpp
    bionic/pthread_equal.cpp
    bionic/pthread_exit.cpp
    #bionic/pthread_getcpuclockid.cpp
    #bionic/pthread_getschedparam.cpp
    #bionic/pthread_gettid_np.cpp
    #bionic/pthread_internal.cpp
    #bionic/pthread_join.cpp
    bionic/pthread_key.cpp
    #bionic/pthread_kill.cpp
    bionic/pthread_mutex.cpp
    bionic/pthread_once.cpp
    #bionic/pthread_rwlock.cpp
    bionic/pthread_self.cpp
    #bionic/pthread_setname_np.cpp
    #bionic/pthread_setschedparam.cpp
    #bionic/pthread_sigmask.cpp

#libc_thread_atexit_impl_src_files
    bionic/__cxa_thread_atexit_impl.cpp

# libc_bionic_src_files
    bionic/getauxval.cpp
    bionic/getentropy_linux.c
    bionic/sysconf.cpp
)

set(cxa_src_sources
# libc_cxa_src_files
    bionic/__cxa_guard.cpp
    bionic/__cxa_pure_virtual.cpp
    bionic/new.cpp
)

set(libc_dns
# libc_dns
    upstream-netbsd/lib/libc/isc/ev_streams.c
    upstream-netbsd/lib/libc/isc/ev_timers.c
    upstream-netbsd/lib/libc/resolv/mtctxres.c
)

set(libc_freebsd
# libc_upstream_freebsd_src_files
    upstream-freebsd/lib/libc/gen/ldexp.c
    upstream-freebsd/lib/libc/gen/sleep.c
    upstream-freebsd/lib/libc/gen/usleep.c
    upstream-freebsd/lib/libc/stdlib/abs.c
    upstream-freebsd/lib/libc/stdlib/getopt_long.c
    upstream-freebsd/lib/libc/stdlib/imaxabs.c
    upstream-freebsd/lib/libc/stdlib/imaxdiv.c
    upstream-freebsd/lib/libc/stdlib/labs.c
    upstream-freebsd/lib/libc/stdlib/llabs.c
    upstream-freebsd/lib/libc/stdlib/qsort.c
    upstream-freebsd/lib/libc/stdlib/quick_exit.c
    upstream-freebsd/lib/libc/stdlib/realpath.c
    upstream-freebsd/lib/libc/string/wcpcpy.c
    upstream-freebsd/lib/libc/string/wcpncpy.c
    upstream-freebsd/lib/libc/string/wcscasecmp.c
    upstream-freebsd/lib/libc/string/wcscspn.c
    upstream-freebsd/lib/libc/string/wcsdup.c
    upstream-freebsd/lib/libc/string/wcslcat.c
    upstream-freebsd/lib/libc/string/wcsncasecmp.c
    upstream-freebsd/lib/libc/string/wcsncat.c
    upstream-freebsd/lib/libc/string/wcsncmp.c
    upstream-freebsd/lib/libc/string/wcsncpy.c
    upstream-freebsd/lib/libc/string/wcsnlen.c
    upstream-freebsd/lib/libc/string/wcspbrk.c
    upstream-freebsd/lib/libc/string/wcsspn.c
    upstream-freebsd/lib/libc/string/wcstok.c
    upstream-freebsd/lib/libc/string/wmemchr.c
    upstream-freebsd/lib/libc/string/wmemset.c
# libc_freebsd_src_files_arm64
    upstream-freebsd/lib/libc/string/wcscat.c
    upstream-freebsd/lib/libc/string/wcschr.c
    upstream-freebsd/lib/libc/string/wcscmp.c
    upstream-freebsd/lib/libc/string/wcscpy.c
    upstream-freebsd/lib/libc/string/wcslen.c
    upstream-freebsd/lib/libc/string/wcsrchr.c
    upstream-freebsd/lib/libc/string/wmemcmp.c
)

set(libc_netbsd
# libc_upstream_netbsd_src_files
    upstream-netbsd/common/lib/libc/stdlib/random.c
    upstream-netbsd/lib/libc/gen/ftw.c
    upstream-netbsd/lib/libc/gen/nftw.c
    #upstream-netbsd/lib/libc/gen/nice.c
    #upstream-netbsd/lib/libc/gen/popen.c
    upstream-netbsd/lib/libc/gen/psignal.c
    #upstream-netbsd/lib/libc/gen/utime.c
    upstream-netbsd/lib/libc/gen/utmp.c
    upstream-netbsd/lib/libc/inet/nsap_addr.c
    upstream-netbsd/lib/libc/regex/regcomp.c
    upstream-netbsd/lib/libc/regex/regerror.c
    upstream-netbsd/lib/libc/regex/regexec.c
    upstream-netbsd/lib/libc/regex/regfree.c
    upstream-netbsd/lib/libc/stdlib/bsearch.c
    upstream-netbsd/lib/libc/stdlib/div.c
    upstream-netbsd/lib/libc/stdlib/drand48.c
    upstream-netbsd/lib/libc/stdlib/erand48.c
    upstream-netbsd/lib/libc/stdlib/jrand48.c
    upstream-netbsd/lib/libc/stdlib/lcong48.c
    upstream-netbsd/lib/libc/stdlib/ldiv.c
    upstream-netbsd/lib/libc/stdlib/lldiv.c
    upstream-netbsd/lib/libc/stdlib/lrand48.c
    upstream-netbsd/lib/libc/stdlib/mrand48.c
    upstream-netbsd/lib/libc/stdlib/nrand48.c
    upstream-netbsd/lib/libc/stdlib/_rand48.c
    upstream-netbsd/lib/libc/stdlib/rand_r.c
    upstream-netbsd/lib/libc/stdlib/reallocarr.c
    upstream-netbsd/lib/libc/stdlib/seed48.c
    upstream-netbsd/lib/libc/stdlib/srand48.c
    upstream-netbsd/lib/libc/string/memccpy.c
    upstream-netbsd/lib/libc/string/strcasestr.c
    upstream-netbsd/lib/libc/string/strcoll.c
    upstream-netbsd/lib/libc/string/strxfrm.c
)

set(libc_openbsd
# libc_upstream_openbsd_src_files
    upstream-openbsd/lib/libc/crypt/arc4random.c
    upstream-openbsd/lib/libc/crypt/arc4random_uniform.c
    #upstream-openbsd/lib/libc/compat-43/killpg.c
    #upstream-openbsd/lib/libc/gen/alarm.c
    upstream-openbsd/lib/libc/gen/ctype_.c
    #upstream-openbsd/lib/libc/gen/daemon.c
    upstream-openbsd/lib/libc/gen/err.c
    upstream-openbsd/lib/libc/gen/errx.c
    #upstream-openbsd/lib/libc/gen/exec.c
    upstream-openbsd/lib/libc/gen/fnmatch.c
    upstream-openbsd/lib/libc/gen/ftok.c
    upstream-openbsd/lib/libc/gen/getprogname.c
    upstream-openbsd/lib/libc/gen/isctype.c
    upstream-openbsd/lib/libc/gen/setprogname.c
    upstream-openbsd/lib/libc/gen/time.c
    upstream-openbsd/lib/libc/gen/tolower_.c
    upstream-openbsd/lib/libc/gen/toupper_.c
    upstream-openbsd/lib/libc/gen/verr.c
    upstream-openbsd/lib/libc/gen/verrx.c
    upstream-openbsd/lib/libc/gen/vwarn.c
    upstream-openbsd/lib/libc/gen/vwarnx.c
    upstream-openbsd/lib/libc/gen/warn.c
    upstream-openbsd/lib/libc/gen/warnx.c
    upstream-openbsd/lib/libc/locale/btowc.c
    upstream-openbsd/lib/libc/locale/mbrlen.c
    upstream-openbsd/lib/libc/locale/mbstowcs.c
    upstream-openbsd/lib/libc/locale/mbtowc.c
    upstream-openbsd/lib/libc/locale/wcscoll.c
    upstream-openbsd/lib/libc/locale/wcstod.c
    upstream-openbsd/lib/libc/locale/wcstof.c
    upstream-openbsd/lib/libc/locale/wcstoimax.c
    upstream-openbsd/lib/libc/locale/wcstol.c
    upstream-openbsd/lib/libc/locale/wcstold.c
    upstream-openbsd/lib/libc/locale/wcstoll.c
    upstream-openbsd/lib/libc/locale/wcstombs.c
    upstream-openbsd/lib/libc/locale/wcstoul.c
    upstream-openbsd/lib/libc/locale/wcstoull.c
    upstream-openbsd/lib/libc/locale/wcstoumax.c
    upstream-openbsd/lib/libc/locale/wcsxfrm.c
    upstream-openbsd/lib/libc/locale/wctob.c
    upstream-openbsd/lib/libc/locale/wctomb.c
    upstream-openbsd/lib/libc/net/htonl.c
    upstream-openbsd/lib/libc/net/htons.c
    upstream-openbsd/lib/libc/net/inet_addr.c
    upstream-openbsd/lib/libc/net/inet_lnaof.c
    upstream-openbsd/lib/libc/net/inet_makeaddr.c
    upstream-openbsd/lib/libc/net/inet_netof.c
    upstream-openbsd/lib/libc/net/inet_network.c
    upstream-openbsd/lib/libc/net/inet_ntoa.c
    upstream-openbsd/lib/libc/net/inet_ntop.c
    upstream-openbsd/lib/libc/net/inet_pton.c
    upstream-openbsd/lib/libc/net/ntohl.c
    upstream-openbsd/lib/libc/net/ntohs.c
    upstream-openbsd/lib/libc/stdio/asprintf.c
    upstream-openbsd/lib/libc/stdio/clrerr.c
    upstream-openbsd/lib/libc/stdio/dprintf.c
    upstream-openbsd/lib/libc/stdio/fclose.c
    upstream-openbsd/lib/libc/stdio/fdopen.c
    upstream-openbsd/lib/libc/stdio/feof.c
    upstream-openbsd/lib/libc/stdio/ferror.c
    upstream-openbsd/lib/libc/stdio/fflush.c
    upstream-openbsd/lib/libc/stdio/fgetc.c
    upstream-openbsd/lib/libc/stdio/fgetln.c
    upstream-openbsd/lib/libc/stdio/fgetpos.c
    upstream-openbsd/lib/libc/stdio/fgets.c
    upstream-openbsd/lib/libc/stdio/fgetwc.c
    upstream-openbsd/lib/libc/stdio/fgetws.c
    upstream-openbsd/lib/libc/stdio/fileno.c
    upstream-openbsd/lib/libc/stdio/flags.c
    upstream-openbsd/lib/libc/stdio/fmemopen.c
    upstream-openbsd/lib/libc/stdio/fopen.c
    upstream-openbsd/lib/libc/stdio/fprintf.c
    upstream-openbsd/lib/libc/stdio/fpurge.c
    upstream-openbsd/lib/libc/stdio/fputc.c
    upstream-openbsd/lib/libc/stdio/fputs.c
    upstream-openbsd/lib/libc/stdio/fputwc.c
    upstream-openbsd/lib/libc/stdio/fputws.c
    upstream-openbsd/lib/libc/stdio/freopen.c
    upstream-openbsd/lib/libc/stdio/fscanf.c
    upstream-openbsd/lib/libc/stdio/fseek.c
    upstream-openbsd/lib/libc/stdio/fsetpos.c
    upstream-openbsd/lib/libc/stdio/ftell.c
    upstream-openbsd/lib/libc/stdio/funopen.c
    upstream-openbsd/lib/libc/stdio/fvwrite.c
    upstream-openbsd/lib/libc/stdio/fwalk.c
    upstream-openbsd/lib/libc/stdio/fwide.c
    upstream-openbsd/lib/libc/stdio/fwprintf.c
    upstream-openbsd/lib/libc/stdio/fwrite.c
    upstream-openbsd/lib/libc/stdio/fwscanf.c
    upstream-openbsd/lib/libc/stdio/getc.c
    upstream-openbsd/lib/libc/stdio/getchar.c
    upstream-openbsd/lib/libc/stdio/getdelim.c
    upstream-openbsd/lib/libc/stdio/getline.c
    upstream-openbsd/lib/libc/stdio/gets.c
    upstream-openbsd/lib/libc/stdio/getwc.c
    upstream-openbsd/lib/libc/stdio/getwchar.c
    upstream-openbsd/lib/libc/stdio/makebuf.c
    upstream-openbsd/lib/libc/stdio/mktemp.c
    upstream-openbsd/lib/libc/stdio/open_memstream.c
    upstream-openbsd/lib/libc/stdio/open_wmemstream.c
    upstream-openbsd/lib/libc/stdio/perror.c
    upstream-openbsd/lib/libc/stdio/printf.c
    upstream-openbsd/lib/libc/stdio/putc.c
    upstream-openbsd/lib/libc/stdio/putchar.c
    upstream-openbsd/lib/libc/stdio/puts.c
    upstream-openbsd/lib/libc/stdio/putwc.c
    upstream-openbsd/lib/libc/stdio/putwchar.c
    upstream-openbsd/lib/libc/stdio/refill.c
    upstream-openbsd/lib/libc/stdio/remove.c
    upstream-openbsd/lib/libc/stdio/rewind.c
    upstream-openbsd/lib/libc/stdio/rget.c
    upstream-openbsd/lib/libc/stdio/scanf.c
    upstream-openbsd/lib/libc/stdio/setbuf.c
    upstream-openbsd/lib/libc/stdio/setbuffer.c
    upstream-openbsd/lib/libc/stdio/setvbuf.c
    upstream-openbsd/lib/libc/stdio/sscanf.c
    upstream-openbsd/lib/libc/stdio/swprintf.c
    upstream-openbsd/lib/libc/stdio/swscanf.c
    upstream-openbsd/lib/libc/stdio/tempnam.c
    upstream-openbsd/lib/libc/stdio/tmpnam.c
    upstream-openbsd/lib/libc/stdio/ungetc.c
    upstream-openbsd/lib/libc/stdio/ungetwc.c
    upstream-openbsd/lib/libc/stdio/vasprintf.c
    upstream-openbsd/lib/libc/stdio/vdprintf.c
    upstream-openbsd/lib/libc/stdio/vfprintf.c
    upstream-openbsd/lib/libc/stdio/vfscanf.c
    upstream-openbsd/lib/libc/stdio/vfwprintf.c
    upstream-openbsd/lib/libc/stdio/vfwscanf.c
    upstream-openbsd/lib/libc/stdio/vprintf.c
    upstream-openbsd/lib/libc/stdio/vscanf.c
    upstream-openbsd/lib/libc/stdio/vsnprintf.c
    upstream-openbsd/lib/libc/stdio/vsprintf.c
    upstream-openbsd/lib/libc/stdio/vsscanf.c
    upstream-openbsd/lib/libc/stdio/vswprintf.c
    upstream-openbsd/lib/libc/stdio/vswscanf.c
    upstream-openbsd/lib/libc/stdio/vwprintf.c
    upstream-openbsd/lib/libc/stdio/vwscanf.c
    upstream-openbsd/lib/libc/stdio/wbuf.c
    upstream-openbsd/lib/libc/stdio/wprintf.c
    upstream-openbsd/lib/libc/stdio/wscanf.c
    upstream-openbsd/lib/libc/stdio/wsetup.c
    upstream-openbsd/lib/libc/stdlib/atoi.c
    upstream-openbsd/lib/libc/stdlib/atol.c
    upstream-openbsd/lib/libc/stdlib/atoll.c
    upstream-openbsd/lib/libc/stdlib/getenv.c
    upstream-openbsd/lib/libc/stdlib/insque.c
    upstream-openbsd/lib/libc/stdlib/lsearch.c
    upstream-openbsd/lib/libc/stdlib/reallocarray.c
    upstream-openbsd/lib/libc/stdlib/remque.c
    upstream-openbsd/lib/libc/stdlib/setenv.c
    upstream-openbsd/lib/libc/stdlib/strtoimax.c
    upstream-openbsd/lib/libc/stdlib/strtol.c
    upstream-openbsd/lib/libc/stdlib/strtoll.c
    upstream-openbsd/lib/libc/stdlib/strtoul.c
    upstream-openbsd/lib/libc/stdlib/strtoull.c
    upstream-openbsd/lib/libc/stdlib/strtoumax.c
    #upstream-openbsd/lib/libc/stdlib/system.c
    upstream-openbsd/lib/libc/stdlib/tfind.c
    upstream-openbsd/lib/libc/stdlib/tsearch.c
    upstream-openbsd/lib/libc/string/strcasecmp.c
    upstream-openbsd/lib/libc/string/strcspn.c
    upstream-openbsd/lib/libc/string/strdup.c
    upstream-openbsd/lib/libc/string/strndup.c
    upstream-openbsd/lib/libc/string/strpbrk.c
    upstream-openbsd/lib/libc/string/strsep.c
    upstream-openbsd/lib/libc/string/strspn.c
    upstream-openbsd/lib/libc/string/strstr.c
    upstream-openbsd/lib/libc/string/strtok.c
    upstream-openbsd/lib/libc/string/wmemcpy.c
    upstream-openbsd/lib/libc/string/wcslcpy.c
    upstream-openbsd/lib/libc/string/wcsstr.c
    upstream-openbsd/lib/libc/string/wcswidth.c
# libc_openbsd_src_files_arm64
    upstream-openbsd/lib/libc/string/memrchr.c
    upstream-openbsd/lib/libc/string/stpncpy.c
    upstream-openbsd/lib/libc/string/strcat.c
    upstream-openbsd/lib/libc/string/strlcat.c
    upstream-openbsd/lib/libc/string/strlcpy.c
    upstream-openbsd/lib/libc/string/strncat.c
    upstream-openbsd/lib/libc/string/strncpy.c
)

set(libc_gdtoa
    upstream-openbsd/android/gdtoa_support.cpp
    upstream-openbsd/lib/libc/gdtoa/dmisc.c
    upstream-openbsd/lib/libc/gdtoa/dtoa.c
    upstream-openbsd/lib/libc/gdtoa/gdtoa.c
    upstream-openbsd/lib/libc/gdtoa/gethex.c
    upstream-openbsd/lib/libc/gdtoa/gmisc.c
    upstream-openbsd/lib/libc/gdtoa/hd_init.c
    upstream-openbsd/lib/libc/gdtoa/hdtoa.c
    upstream-openbsd/lib/libc/gdtoa/hexnan.c
    upstream-openbsd/lib/libc/gdtoa/ldtoa.c
    upstream-openbsd/lib/libc/gdtoa/misc.c
    upstream-openbsd/lib/libc/gdtoa/smisc.c
    upstream-openbsd/lib/libc/gdtoa/strtod.c
    upstream-openbsd/lib/libc/gdtoa/strtodg.c
    upstream-openbsd/lib/libc/gdtoa/strtof.c
    upstream-openbsd/lib/libc/gdtoa/strtord.c
    upstream-openbsd/lib/libc/gdtoa/sum.c
    upstream-openbsd/lib/libc/gdtoa/ulp.c
    upstream-openbsd/lib/libc/gdtoa/strtorQ.c
)

set(libc_tzcode
    tzcode/asctime.c
    tzcode/difftime.c
    tzcode/localtime.c
    tzcode/strftime.c
    tzcode/strptime.c
    upstream-openbsd/lib/libc/time/wcsftime.c
)

list(TRANSFORM libc_sources PREPEND bionic/libc/)
list(TRANSFORM cxa_src_sources PREPEND bionic/libc/)
list(TRANSFORM libc_dns PREPEND bionic/libc/)
list(TRANSFORM libc_freebsd PREPEND bionic/libc/)
list(TRANSFORM libc_netbsd PREPEND bionic/libc/)
list(TRANSFORM libc_openbsd PREPEND bionic/libc/)
list(TRANSFORM libc_gdtoa PREPEND bionic/libc/)
list(TRANSFORM libc_tzcode PREPEND bionic/libc/)

add_library(bionic SHARED
    ${libc_sources}
    ${libc_dns}
    ${libc_freebsd}
    ${libc_netbsd}
    ${libc_openbsd}
    ${libc_gdtoa}
    ${libc_tzcode}
    ${cxa_src_sources}

    compatibility_runtime_unification.c
    call_arm_code_catching_exceptions.S
    exception_handling.c
)

target_compile_definitions(bionic PRIVATE
    -D_LIBC=1

    # CR: We do *not* use logd.
    # -DTARGET_USES_LOGD

    -DUSE_DLMALLOC

    -DANDROID_SMP=1

    -DCOMPATIBILITY_RUNTIME_BUILD
)


target_compile_options(bionic PRIVATE
    -Wall -Wextra -Wunused

    # Try to catch typical 32-bit assumptions that break with 64-bit pointers.
    -Werror=pointer-to-int-cast
    -Werror=int-to-pointer-cast
    -Werror=type-limits

    $<$<COMPILE_LANGUAGE:CXX>:-nostdinc++>
    -fno-exceptions
)

target_include_directories(bionic SYSTEM PUBLIC
    bionic/libc/include
    bionic/libc/arch-arm64/include
    bionic/libc/kernel/uapi
    bionic/libc/kernel/uapi/asm-arm64

    bionic/libm/include
    bionic/libm/include/arm64
)

target_include_directories(bionic PRIVATE
    bionic/libc
    bionic/libc/stdio
    $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/bionic/libstdc++/include>
)

target_link_options(bionic PRIVATE
    -nostdlib
    -nostartfiles
    -Wl,-z,defs
    -Wl,--build-id=none
    -Wl,--hash-style=sysv
    -Wl,-Bsymbolic
    -Wl,-z,norelro
    -Wl,-s
)

target_link_libraries(bionic PRIVATE
    -Wl,-Bstatic,--exclude-libs,libgcc.a,-lgcc,-Bdynamic
)

set_target_properties(bionic PROPERTIES PREFIX "")

set_source_files_properties(${cxa_src_sources} PROPERTIES COMPILE_OPTIONS -fvisibility=hidden)

set_source_files_properties(${libc_dns} PROPERTIES COMPILE_DEFINITIONS "-DANDROID_CHANGES;-DINET6")
set_source_files_properties(${libc_dns} PROPERTIES COMPILE_OPTIONS "-fvisibility=hidden;-Wno-unused-parameter;-include;netbsd-compat.h")
set_source_files_properties(${libc_dns} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/dns/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/private;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-netbsd/lib/libc/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-netbsd/android/include")

set_source_files_properties(${libc_freebsd} PROPERTIES COMPILE_OPTIONS "-Wno-sign-compare;-Wno-uninitialized;-include;freebsd-compat.h")
set_source_files_properties(${libc_freebsd} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-freebsd/android/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-freebsd/lib/libc/include")

set_source_files_properties(${libc_netbsd} PROPERTIES COMPILE_OPTIONS "-Wno-sign-compare;-Wno-uninitialized;-include;netbsd-compat.h")
set_source_files_properties(${libc_netbsd} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-netbsd/android/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-netbsd/lib/libc/include")
set_source_files_properties(${libc_netbsd} PROPERTIES COMPILE_DEFINITIONS "-DPOSIX_MISTAKE")

set_source_files_properties(${libc_openbsd} PROPERTIES COMPILE_OPTIONS "-Wno-sign-compare;-Wno-uninitialized;-Wno-unused-parameter;-include;openbsd-compat.h")
set_source_files_properties(${libc_openbsd} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/private;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-openbsd/android/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-openbsd/lib/libc/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-openbsd/lib/libc/gdtoa")

set_source_files_properties(${libc_gdtoa} PROPERTIES COMPILE_OPTIONS "-Wno-sign-compare;-Wno-uninitialized;-fvisibility=hidden;-include;openbsd-compat.h")
set_source_files_properties(${libc_gdtoa} PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/private;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-openbsd/android/include;${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/upstream-openbsd/lib/libc/include")

set_source_files_properties(${libc_tzcode} PROPERTIES
    COMPILE_DEFINITIONS
        "-DALL_STATE;-DSTD_INSPIRED;-DTM_GMTOFF=tm_gmtoff;-DTZDIR=\"/system/usr/share/zoneinfo\";-DUSG_COMPAT=1;-DNO_RUN_TIME_WARNINGS_ABOUT_YEAR_2000_PROBLEMS_THANK_YOU;-Dlint"
    COMPILE_OPTIONS "-fvisibility=hidden"
    INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/bionic/libc/tzcode")

# The stack protector code needs to be compiled
# with -fno-stack-protector, since it modifies the
# stack canary.
set_source_files_properties(bionic/libc/bionic/__stack_chk_fail.cpp PROPERTIES COMPILE_OPTIONS -fno-stack-protector)

set_source_files_properties(bionic/libc/bionic/dlmalloc.c PROPERTIES COMPILE_OPTIONS -fvisibility=hidden)

target_compile_definitions(bionic PRIVATE $<$<COMPILE_LANGUAGE:ASM>:-D__ASSEMBLY__>)

set_source_files_properties(
    bionic/libc/arch-arm64/generic/bionic/memchr.S
    bionic/libc/arch-arm64/generic/bionic/strchr.S
    PROPERTIES COMPILE_OPTIONS -no-integrated-as)

target_link_libraries(bionic PRIVATE compatibility-runtime-platform-stubs)

set(libm_sources
    digittoint.c
    significandl.c
    sincos.c
    upstream-freebsd/lib/msun/bsdsrc/b_exp.c
    upstream-freebsd/lib/msun/bsdsrc/b_log.c
    upstream-freebsd/lib/msun/bsdsrc/b_tgamma.c
    upstream-freebsd/lib/msun/src/e_acos.c
    upstream-freebsd/lib/msun/src/e_acosf.c
    upstream-freebsd/lib/msun/src/e_acosh.c
    upstream-freebsd/lib/msun/src/e_acoshf.c
    upstream-freebsd/lib/msun/src/e_asin.c
    upstream-freebsd/lib/msun/src/e_asinf.c
    upstream-freebsd/lib/msun/src/e_atan2.c
    upstream-freebsd/lib/msun/src/e_atan2f.c
    upstream-freebsd/lib/msun/src/e_atanh.c
    upstream-freebsd/lib/msun/src/e_atanhf.c
    upstream-freebsd/lib/msun/src/e_cosh.c
    upstream-freebsd/lib/msun/src/e_coshf.c
    upstream-freebsd/lib/msun/src/e_exp.c
    upstream-freebsd/lib/msun/src/e_expf.c
    upstream-freebsd/lib/msun/src/e_fmod.c
    upstream-freebsd/lib/msun/src/e_fmodf.c
    upstream-freebsd/lib/msun/src/e_gamma.c
    upstream-freebsd/lib/msun/src/e_gammaf.c
    upstream-freebsd/lib/msun/src/e_gammaf_r.c
    upstream-freebsd/lib/msun/src/e_gamma_r.c
    upstream-freebsd/lib/msun/src/e_hypot.c
    upstream-freebsd/lib/msun/src/e_hypotf.c
    upstream-freebsd/lib/msun/src/e_j0.c
    upstream-freebsd/lib/msun/src/e_j0f.c
    upstream-freebsd/lib/msun/src/e_j1.c
    upstream-freebsd/lib/msun/src/e_j1f.c
    upstream-freebsd/lib/msun/src/e_jn.c
    upstream-freebsd/lib/msun/src/e_jnf.c
    upstream-freebsd/lib/msun/src/e_lgamma.c
    upstream-freebsd/lib/msun/src/e_lgammaf.c
    upstream-freebsd/lib/msun/src/e_lgammaf_r.c
    upstream-freebsd/lib/msun/src/e_lgamma_r.c
    upstream-freebsd/lib/msun/src/e_log10.c
    upstream-freebsd/lib/msun/src/e_log10f.c
    upstream-freebsd/lib/msun/src/e_log2.c
    upstream-freebsd/lib/msun/src/e_log2f.c
    upstream-freebsd/lib/msun/src/e_log.c
    upstream-freebsd/lib/msun/src/e_logf.c
    upstream-freebsd/lib/msun/src/e_pow.c
    upstream-freebsd/lib/msun/src/e_powf.c
    upstream-freebsd/lib/msun/src/e_remainder.c
    upstream-freebsd/lib/msun/src/e_remainderf.c
    upstream-freebsd/lib/msun/src/e_rem_pio2.c
    upstream-freebsd/lib/msun/src/e_rem_pio2f.c
    upstream-freebsd/lib/msun/src/e_scalb.c
    upstream-freebsd/lib/msun/src/e_scalbf.c
    upstream-freebsd/lib/msun/src/e_sinh.c
    upstream-freebsd/lib/msun/src/e_sinhf.c
    upstream-freebsd/lib/msun/src/e_sqrt.c
    upstream-freebsd/lib/msun/src/e_sqrtf.c
    upstream-freebsd/lib/msun/src/imprecise.c
    upstream-freebsd/lib/msun/src/k_cos.c
    upstream-freebsd/lib/msun/src/k_cosf.c
    upstream-freebsd/lib/msun/src/k_exp.c
    upstream-freebsd/lib/msun/src/k_expf.c
    upstream-freebsd/lib/msun/src/k_rem_pio2.c
    upstream-freebsd/lib/msun/src/k_sin.c
    upstream-freebsd/lib/msun/src/k_sinf.c
    upstream-freebsd/lib/msun/src/k_tan.c
    upstream-freebsd/lib/msun/src/k_tanf.c
    upstream-freebsd/lib/msun/src/s_asinh.c
    upstream-freebsd/lib/msun/src/s_asinhf.c
    upstream-freebsd/lib/msun/src/s_atan.c
    upstream-freebsd/lib/msun/src/s_atanf.c
    upstream-freebsd/lib/msun/src/s_carg.c
    upstream-freebsd/lib/msun/src/s_cargf.c
    upstream-freebsd/lib/msun/src/s_cbrt.c
    upstream-freebsd/lib/msun/src/s_cbrtf.c
    upstream-freebsd/lib/msun/src/s_ccosh.c
    upstream-freebsd/lib/msun/src/s_ccoshf.c
    upstream-freebsd/lib/msun/src/s_ceil.c
    upstream-freebsd/lib/msun/src/s_ceilf.c
    upstream-freebsd/lib/msun/src/s_cexp.c
    upstream-freebsd/lib/msun/src/s_cexpf.c
    upstream-freebsd/lib/msun/src/s_cimag.c
    upstream-freebsd/lib/msun/src/s_cimagf.c
    upstream-freebsd/lib/msun/src/s_conj.c
    upstream-freebsd/lib/msun/src/s_conjf.c
    upstream-freebsd/lib/msun/src/s_copysign.c
    upstream-freebsd/lib/msun/src/s_copysignf.c
    upstream-freebsd/lib/msun/src/s_cos.c
    upstream-freebsd/lib/msun/src/s_cosf.c
    upstream-freebsd/lib/msun/src/s_cproj.c
    upstream-freebsd/lib/msun/src/s_cprojf.c
    upstream-freebsd/lib/msun/src/s_creal.c
    upstream-freebsd/lib/msun/src/s_crealf.c
    upstream-freebsd/lib/msun/src/s_csinh.c
    upstream-freebsd/lib/msun/src/s_csinhf.c
    upstream-freebsd/lib/msun/src/s_csqrt.c
    upstream-freebsd/lib/msun/src/s_csqrtf.c
    upstream-freebsd/lib/msun/src/s_ctanh.c
    upstream-freebsd/lib/msun/src/s_ctanhf.c
    upstream-freebsd/lib/msun/src/s_erf.c
    upstream-freebsd/lib/msun/src/s_erff.c
    upstream-freebsd/lib/msun/src/s_exp2.c
    upstream-freebsd/lib/msun/src/s_exp2f.c
    upstream-freebsd/lib/msun/src/s_expm1.c
    upstream-freebsd/lib/msun/src/s_expm1f.c
    upstream-freebsd/lib/msun/src/s_fabs.c
    upstream-freebsd/lib/msun/src/s_fabsf.c
    upstream-freebsd/lib/msun/src/s_fdim.c
    upstream-freebsd/lib/msun/src/s_finite.c
    upstream-freebsd/lib/msun/src/s_finitef.c
    upstream-freebsd/lib/msun/src/s_floor.c
    upstream-freebsd/lib/msun/src/s_floorf.c
    upstream-freebsd/lib/msun/src/s_fma.c
    upstream-freebsd/lib/msun/src/s_fmaf.c
    upstream-freebsd/lib/msun/src/s_fmax.c
    upstream-freebsd/lib/msun/src/s_fmaxf.c
    upstream-freebsd/lib/msun/src/s_fmin.c
    upstream-freebsd/lib/msun/src/s_fminf.c
    upstream-freebsd/lib/msun/src/s_frexp.c
    upstream-freebsd/lib/msun/src/s_frexpf.c
    upstream-freebsd/lib/msun/src/s_ilogb.c
    upstream-freebsd/lib/msun/src/s_ilogbf.c
    upstream-freebsd/lib/msun/src/s_llrint.c
    upstream-freebsd/lib/msun/src/s_llrintf.c
    upstream-freebsd/lib/msun/src/s_llround.c
    upstream-freebsd/lib/msun/src/s_llroundf.c
    upstream-freebsd/lib/msun/src/s_log1p.c
    upstream-freebsd/lib/msun/src/s_log1pf.c
    upstream-freebsd/lib/msun/src/s_logb.c
    upstream-freebsd/lib/msun/src/s_logbf.c
    upstream-freebsd/lib/msun/src/s_lrint.c
    upstream-freebsd/lib/msun/src/s_lrintf.c
    upstream-freebsd/lib/msun/src/s_lround.c
    upstream-freebsd/lib/msun/src/s_lroundf.c
    upstream-freebsd/lib/msun/src/s_modf.c
    upstream-freebsd/lib/msun/src/s_modff.c
    upstream-freebsd/lib/msun/src/s_nan.c
    upstream-freebsd/lib/msun/src/s_nearbyint.c
    upstream-freebsd/lib/msun/src/s_nextafter.c
    upstream-freebsd/lib/msun/src/s_nextafterf.c
    upstream-freebsd/lib/msun/src/s_remquo.c
    upstream-freebsd/lib/msun/src/s_remquof.c
    upstream-freebsd/lib/msun/src/s_rint.c
    upstream-freebsd/lib/msun/src/s_rintf.c
    upstream-freebsd/lib/msun/src/s_round.c
    upstream-freebsd/lib/msun/src/s_roundf.c
    upstream-freebsd/lib/msun/src/s_scalbln.c
    upstream-freebsd/lib/msun/src/s_scalbn.c
    upstream-freebsd/lib/msun/src/s_scalbnf.c
    upstream-freebsd/lib/msun/src/s_signgam.c
    upstream-freebsd/lib/msun/src/s_significand.c
    upstream-freebsd/lib/msun/src/s_significandf.c
    upstream-freebsd/lib/msun/src/s_sin.c
    upstream-freebsd/lib/msun/src/s_sinf.c
    upstream-freebsd/lib/msun/src/s_tan.c
    upstream-freebsd/lib/msun/src/s_tanf.c
    upstream-freebsd/lib/msun/src/s_tanh.c
    upstream-freebsd/lib/msun/src/s_tanhf.c
    upstream-freebsd/lib/msun/src/s_tgammaf.c
    upstream-freebsd/lib/msun/src/s_trunc.c
    upstream-freebsd/lib/msun/src/s_truncf.c
    upstream-freebsd/lib/msun/src/w_cabs.c
    upstream-freebsd/lib/msun/src/w_cabsf.c
    upstream-freebsd/lib/msun/src/w_drem.c
    upstream-freebsd/lib/msun/src/w_dremf.c
    fake_long_double.c
    signbit.c
    arm64/fenv.c
    upstream-freebsd/lib/msun/src/e_acosl.c
    upstream-freebsd/lib/msun/src/e_acoshl.c
    upstream-freebsd/lib/msun/src/e_asinl.c
    upstream-freebsd/lib/msun/src/e_atan2l.c
    upstream-freebsd/lib/msun/src/e_atanhl.c
    upstream-freebsd/lib/msun/src/e_fmodl.c
    upstream-freebsd/lib/msun/src/e_hypotl.c
    upstream-freebsd/lib/msun/src/e_remainderl.c
    upstream-freebsd/lib/msun/src/e_sqrtl.c
    upstream-freebsd/lib/msun/src/s_asinhl.c
    upstream-freebsd/lib/msun/src/s_atanl.c
    upstream-freebsd/lib/msun/src/s_cbrtl.c
    upstream-freebsd/lib/msun/src/s_ceill.c
    upstream-freebsd/lib/msun/src/s_copysignl.c
    upstream-freebsd/lib/msun/src/s_cosl.c
    upstream-freebsd/lib/msun/src/s_fabsl.c
    upstream-freebsd/lib/msun/src/s_floorl.c
    upstream-freebsd/lib/msun/src/s_fmal.c
    upstream-freebsd/lib/msun/src/s_fmaxl.c
    upstream-freebsd/lib/msun/src/s_fminl.c
    upstream-freebsd/lib/msun/src/s_modfl.c
    upstream-freebsd/lib/msun/src/s_frexpl.c
    upstream-freebsd/lib/msun/src/s_ilogbl.c
    upstream-freebsd/lib/msun/src/s_llrintl.c
    upstream-freebsd/lib/msun/src/s_llroundl.c
    upstream-freebsd/lib/msun/src/s_logbl.c
    upstream-freebsd/lib/msun/src/s_lrintl.c
    upstream-freebsd/lib/msun/src/s_lroundl.c
    upstream-freebsd/lib/msun/src/s_nextafterl.c
    upstream-freebsd/lib/msun/src/s_nexttoward.c
    upstream-freebsd/lib/msun/src/s_nexttowardf.c
    upstream-freebsd/lib/msun/src/s_remquol.c
    upstream-freebsd/lib/msun/src/s_rintl.c
    upstream-freebsd/lib/msun/src/s_roundl.c
    upstream-freebsd/lib/msun/src/s_scalbnl.c
    upstream-freebsd/lib/msun/src/s_sinl.c
    upstream-freebsd/lib/msun/src/s_tanl.c
    upstream-freebsd/lib/msun/src/s_truncl.c
    upstream-freebsd/lib/msun/ld128/invtrig.c
    upstream-freebsd/lib/msun/ld128/k_cosl.c
    upstream-freebsd/lib/msun/ld128/k_sinl.c
    upstream-freebsd/lib/msun/ld128/k_tanl.c
    upstream-freebsd/lib/msun/ld128/s_exp2l.c
    upstream-freebsd/lib/msun/ld128/s_expl.c
    upstream-freebsd/lib/msun/ld128/s_logl.c
    upstream-freebsd/lib/msun/ld128/s_nanl.c
)

list(TRANSFORM libm_sources PREPEND bionic/libm/)

add_library(bionic-math OBJECT
    ${libm_sources}
)
target_compile_definitions(bionic-math PRIVATE -DFLT_EVAL_METHOD=0)
target_compile_options(bionic-math PRIVATE -include ${CMAKE_CURRENT_SOURCE_DIR}/bionic/libm/freebsd-compat.h -Wno-missing-braces
    -Wno-parentheses -Wno-sign-compare -Wno-uninitialized -Wno-unknown-pragmas -fvisibility=hidden -fno-builtin-rint -fno-builtin-rintf -fno-builtin-rintl
    -U__ANDROID_API__)
target_include_directories(bionic-math PRIVATE $<TARGET_PROPERTY:bionic,INTERFACE_INCLUDE_DIRECTORIES> bionic/libm
    bionic/libm/upstream-freebsd/lib/msun/src
    bionic/libm/upstream-freebsd/lib/msun/ld128
)

target_include_directories(bionic-math SYSTEM PRIVATE
    bionic/libm/include
    bionic/libm/include/arm64
)

target_sources(bionic PRIVATE $<TARGET_OBJECTS:bionic-math>)


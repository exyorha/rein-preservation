.text
.global _compatibility_runtime_stub
.hidden _compatibility_runtime_stub
_compatibility_runtime_stub:
    svc #0xBAD

.type _compatibility_runtime_stub, %function
.size _compatibility_runtime_stub, . -_compatibility_runtime_stub

.globl bind
bind = _compatibility_runtime_stub

.globl fstatat
fstatat = _compatibility_runtime_stub

.globl fchdir
fchdir = _compatibility_runtime_stub

.globl fstat
fstat = _compatibility_runtime_stub

.globl __ioctl
__ioctl = _compatibility_runtime_stub

.globl lseek
lseek = _compatibility_runtime_stub

.globl read
read = _compatibility_runtime_stub

.globl write
write = _compatibility_runtime_stub

.globl ___close
___close = _compatibility_runtime_stub

.globl __getdents64
__getdents64 = _compatibility_runtime_stub

.globl ___faccessat
___faccessat = _compatibility_runtime_stub

.globl __accept4
__accept4 = _compatibility_runtime_stub

.globl __connect
__connect = _compatibility_runtime_stub

.globl __socket
__socket = _compatibility_runtime_stub

.globl __openat
__openat = _compatibility_runtime_stub

.globl pipe2
pipe2 = _compatibility_runtime_stub

.globl fcntl
fcntl = _compatibility_runtime_stub

.globl dup3
dup3 = _compatibility_runtime_stub

.globl mprotect
mprotect = _compatibility_runtime_stub

.globl mmap
mmap = _compatibility_runtime_stub

.globl munmap
munmap = _compatibility_runtime_stub

.globl readlinkat
readlinkat = _compatibility_runtime_stub

.globl clock_getres
clock_getres = _compatibility_runtime_stub

.globl clock_gettime
clock_gettime = _compatibility_runtime_stub

.globl recvfrom
recvfrom = _compatibility_runtime_stub

.globl __getcwd
__getcwd = _compatibility_runtime_stub

.globl __ppoll
__ppoll = _compatibility_runtime_stub

.globl __pselect6
__pselect6 = _compatibility_runtime_stub

.globl sendto
sendto = _compatibility_runtime_stub

.globl __rt_sigaction
__rt_sigaction = _compatibility_runtime_stub

.globl __rt_sigsuspend
__rt_sigsuspend = _compatibility_runtime_stub

.globl ___getpid
___getpid = _compatibility_runtime_stub

.globl getuid
getuid = _compatibility_runtime_stub

.globl mkdirat
mkdirat = _compatibility_runtime_stub

.globl renameat
renameat = _compatibility_runtime_stub

.globl unlinkat
unlinkat = _compatibility_runtime_stub

.globl pthread_mutex_init
pthread_mutex_init = _compatibility_runtime_stub

.globl pthread_mutex_lock
pthread_mutex_lock = _compatibility_runtime_stub

.globl pthread_mutex_trylock
pthread_mutex_trylock = _compatibility_runtime_stub

.globl pthread_mutex_unlock
pthread_mutex_unlock = _compatibility_runtime_stub

.globl pthread_mutex_destroy
pthread_mutex_destroy = _compatibility_runtime_stub

.globl pthread_kill
pthread_kill = _compatibility_runtime_stub

.globl __compatibility_runtime_write_log
__compatibility_runtime_write_log = _compatibility_runtime_stub

.globl __compatibility_runtime_log_event
__compatibility_runtime_log_event = _compatibility_runtime_stub

.globl __system_property_get
__system_property_get = _compatibility_runtime_stub

.globl gettimeofday
gettimeofday = _compatibility_runtime_stub

.globl nanosleep
nanosleep = _compatibility_runtime_stub

.globl _Exit
_Exit = _compatibility_runtime_stub

.globl writev
writev = _compatibility_runtime_stub

.globl sched_yield
sched_yield = _compatibility_runtime_stub

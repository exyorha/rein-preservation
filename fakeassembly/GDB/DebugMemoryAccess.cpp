#include <GDB/DebugMemoryAccess.h>

#include <Translator/JIT.h>

#include <signal.h>
#include <system_error>
#include <setjmp.h>
#include <sys/mman.h>

static jmp_buf recoverFromDebugFault;

static void debugMemoryAccessFault(int signal) {
    (void)signal;
    siglongjmp(recoverFromDebugFault, 1);
}

bool debugMemoryRead(uintptr_t address, void *dest, size_t length) {
    /*
     * We temporarily catch SIGSEGV while executing this. While this would
     * otherwise interfere with dynarmic, this is only used from the GDB
     * stub, which only operates with the JIT inactive and the global JIT
     * lock held.
     */

    struct sigaction oldAction;

    struct sigaction action;
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = debugMemoryAccessFault;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGSEGV, &action, &oldAction) < 0)
        throw std::system_error(errno, std::generic_category());

    bool success = true;

    if(sigsetjmp(recoverFromDebugFault, 1)) {
        /*
         * A fault has occurred.
         */
        success = false;
    } else {
        memcpy(dest, reinterpret_cast<const void *>(address), length);
    }

    if(sigaction(SIGSEGV, &oldAction, nullptr) < 0)
        throw std::system_error(errno, std::generic_category());

    return success;
}

bool debugMemoryWrite(uintptr_t address, const void *src, size_t length) {
    /*
     * We temporarily catch SIGSEGV while executing this. While this would
     * otherwise interfere with dynarmic, this is only used from the GDB
     * stub, which only operates with the JIT inactive and the global JIT
     * lock held.
     */

    {
        uintptr_t misalign = address & 4095;
        mprotect(reinterpret_cast<void *>(address - misalign), length + misalign, PROT_READ | PROT_WRITE | PROT_EXEC);
    }

    struct sigaction oldAction;

    struct sigaction action;
    action.sa_flags = SA_RESETHAND;
    action.sa_handler = debugMemoryAccessFault;
    sigemptyset(&action.sa_mask);
    if(sigaction(SIGSEGV, &action, &oldAction) < 0)
        throw std::system_error(errno, std::generic_category());

    bool success = true;

    if(sigsetjmp(recoverFromDebugFault, 1)) {
        /*
         * A fault has occurred.
         */
        success = false;
    } else {
        memcpy(reinterpret_cast<void *>(address), src, length);
    }

    if(sigaction(SIGSEGV, &oldAction, nullptr) < 0)
        throw std::system_error(errno, std::generic_category());

    JIT::flushInstructionCacheLockedInternal(address, length);

    return success;
}

#include <Bionic/BionicSyscalls.h>

int translateErrno(int hostPlatformError) {
    return hostPlatformError;
}

bool translateClockID(clockid_t &host, bionic_clockid_t bionic) {
    host = bionic;
    return true;
}

int translateOpenFlag(int flag) {
    return flag;
}

int translateAccessMode(int mode) {
    return mode;
}

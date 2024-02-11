#include <Bionic/BionicCallouts.h>
#include <ELF/Image.h>

BionicInitLibcType arm_bionic_init_libc;
BionicTeardownThread arm_bionic_teardown_thread;

void bindBionicCallouts(const Image &bionic) {
    arm_bionic_init_libc = reinterpret_cast<BionicInitLibcType>(bionic.getSymbolChecked("__compatibility_runtime_init_libc"));
    arm_bionic_teardown_thread = reinterpret_cast<BionicTeardownThread>(bionic.getSymbolChecked("__compatibility_runtime_teardown_thread"));
}

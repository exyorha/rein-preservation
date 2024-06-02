#include <Bionic/BionicCallouts.h>
#include <ELF/ElfSymbolSource.h>

BionicInitLibcType arm_bionic_init_libc;
BionicTeardownThread arm_bionic_teardown_thread;
void *arm_bionic_armcall_catcher;
void *arm_bionic_armcall_rethrower;

void bindBionicCallouts(const ElfSymbolSource &bionic) {
    arm_bionic_init_libc = reinterpret_cast<BionicInitLibcType>(bionic.getSymbolChecked("__compatibility_runtime_init_libc"));
    arm_bionic_teardown_thread = reinterpret_cast<BionicTeardownThread>(bionic.getSymbolChecked("__compatibility_runtime_teardown_thread"));
    arm_bionic_armcall_catcher = bionic.getSymbolChecked("__compatibility_runtime_armcall_catcher");
    arm_bionic_armcall_rethrower = bionic.getSymbolChecked("__compatibility_runtime_armcall_rethrower");
}

.arch armv8-a

.text
.globl __compatibility_runtime_armcall_catcher
__compatibility_runtime_armcall_catcher:
    .cfi_startproc
    .cfi_personality 0x9b, ref_compatibility_runtime_armcall_personality

    // It's the responsibility of the translator to perform the equivalent of
    // the instructions marked with 'TRANSLATOR'.

    nop

    //TRANSLATOR:stp	x29, x30, [sp, -48]!

    .cfi_def_cfa_offset 48
    .cfi_offset 29, -48
	.cfi_offset 30, -40

	//TRANSLATOR:mov	x29, sp
	.cfi_def_cfa_register 29

	//TRANSLATOR:blr <CALL TARGET>
	svc #0xE0

	//TRANSLATOR:ldp	x29, x30, [sp], 48
    .cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa 31, 0
	ret

    .cfi_endproc

.type __compatibility_runtime_armcall_catcher, %function
.size __compatibility_runtime_armcall_catcher, . - __compatibility_runtime_armcall_catcher

.globl __compatibility_runtime_armcall_exception
__compatibility_runtime_armcall_exception:
    svc #0xE1
.type __compatibility_runtime_armcall_exception, %function
.size __compatibility_runtime_armcall_exception, . - __compatibility_runtime_armcall_exception

.data

ref_compatibility_runtime_armcall_personality:
.xword __compatibility_runtime_armcall_personality
.type ref_compatibility_runtime_armcall_personality, %object
.size ref_compatibility_runtime_armcall_personality, . - ref_compatibility_runtime_armcall_personality

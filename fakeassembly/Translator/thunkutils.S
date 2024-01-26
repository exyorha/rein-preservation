.text

.balign 16
.globl getOffsetOfThunkUtilitySlot
getOffsetOfThunkUtilitySlot:
    # For static model:
    # movq thunkUtilitySlot@GOTTPOFF(%rip), %rax

    leaq thunkUtilitySlot@TLSDESC(%rip), %rax
    call *thunkUtilitySlot@TLSCALL(%rax)

    ret

.type getOffsetOfThunkUtilitySlot,%function
.size getOffsetOfThunkUtilitySlot, . - getOffsetOfThunkUtilitySlot

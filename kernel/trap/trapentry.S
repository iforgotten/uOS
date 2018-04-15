.text
.global __alltraps
__alltraps:
	pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushal

    movl $16, %eax
    mov %ax, %ds
    mov %ax, %es

    pushl %esp

    call trap

    popl %esp

.global __trapret
__trapret:
    popal

    pop %gs
    pop %fs
    pop %es
    pop %ds

    addl $0x8, %esp
    iret

#include "memlayout.h"
.text
.global __alltraps
__alltraps:
	pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushal

    movl $GD_KDATA, %eax
    movw %ax, %ds
    movw %ax, %es

    pushl %esp

    call trap

    popl %esp

.global __trapret
__trapret:
    popal

    popl %gs
    popl %fs
    popl %es
    popl %ds

    addl $0x8, %esp
    iret

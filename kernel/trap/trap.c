/*
 * trap.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#include "trap.h"
#include "mmu.h"
#include "memlayout.h"
#include "x86.h"
#include "stdio.h"
#include "clock.h"
#include "string.h"

#define TICK_NUM 100

static struct GateDescriptors IDT[256] = {{0}};
static struct pseudodesc IDT_PD = {
    sizeof(IDT) - 1, (uintptr_t)IDT
};

struct trapframe switchk2u, *switchu2k;

static void
trap_dispatch(struct trapframe* tf) {
	uint32_t no = tf->tf_trapno;
	switch(no) {
	case (IRQ_OFFSET + IRQ_TIMER):
		++ticks;
		if(ticks % TICK_NUM == 0) {
			cprintf("TIMER INTERRUPT\n");
		}
		break;
	case TRAP_SWITCH_TO_USR:
		if(tf->tf_cs != USER_CS) {
            switchk2u = *tf;
            switchk2u.tf_cs = USER_CS;
            switchk2u.tf_ds = switchk2u.tf_es = switchk2u.tf_ss = USER_DS;
            switchk2u.tf_esp = (uint32_t)tf + sizeof(struct trapframe) - 8;

            switchk2u.tf_eflags |= EFL_IOPL;
            *((uint32_t *)tf - 1) = (uint32_t)&switchk2u;
		}
		break;
	case TRAP_SWITCH_TO_KERN:
        if (tf->tf_cs != KERNEL_CS) {
            tf->tf_cs = KERNEL_CS;
            tf->tf_ds = tf->tf_es = KERNEL_DS;
            tf->tf_eflags &= ~EFL_IOPL;

            switchu2k = (struct trapframe *)(tf->tf_esp - (sizeof(struct trapframe) - 8));
            memmove(switchu2k, tf, sizeof(struct trapframe) - 8);

            *((uint32_t *)tf - 1) = (uint32_t)switchu2k;
        }
		break;
	default:
		// 在内核态，这里出现的就是错误
		break;
	}
}

void
idt_init(void) {
	extern uintptr_t __vectors[];
	// 构造IDT
	int nSize = sizeof(IDT) / sizeof(struct GateDescriptors);
	int i;
	for(i = 0; i < nSize; ++i) {
		SETGATE(IDT[i], 0, GD_KTEXT,__vectors[i], DPL_KERNEL);
	}
	// 设置从3环切换到0环的中断描述符
	SETGATE(IDT[TRAP_SWITCH_TO_USR], 0, GD_KTEXT,__vectors[TRAP_SWITCH_TO_USR], DPL_USER);
	SETGATE(IDT[TRAP_SWITCH_TO_KERN], 0, GD_KTEXT,__vectors[TRAP_SWITCH_TO_KERN], DPL_USER);
	lidt(&IDT_PD);
}

void
trap(struct trapframe* tf) {
	trap_dispatch(tf);
}

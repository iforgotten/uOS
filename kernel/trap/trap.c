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

#define TICK_NUM 100

static struct GateDescriptors IDT[256] = {{0}};
static struct pseudodesc IDT_PD = {
    sizeof(IDT) - 1, (uintptr_t)IDT
};

static void
trap_dispatch(struct trapframe* tf) {
	uint32_t no = tf->tf_trapno;
	switch(no) {
	case (IRQ_OFFSET + IRQ_TIMER):
		// cprintf("TIMER INTERRUPT\n");
		++ticks;
		if(ticks % TICK_NUM == 0) {
			cprintf("TIMER INTERRUPT\n");
		}
		break;
	default:
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
		// SETGATE(gate,istrap,selector,offset,dpl)
		SETGATE(IDT[i], 0, GD_KTEXT,__vectors[i], DPL_KERNEL);
	}
	// 设置从3环切换到0环的中断描述符
	SETGATE(IDT[TRAP_SWITCH_TO_USR], 0, GD_KTEXT,__vectors[TRAP_SWITCH_TO_USR],DPL_USER);

	lidt(&IDT_PD);
}

void
trap(struct trapframe* tf) {
	trap_dispatch(tf);
}

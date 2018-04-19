/*
 * pmm.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */
#include "x86.h"
#include "pmm.h"
#include "memlayout.h"
#include "mmu.h"

static struct TaskState ts = {0};

static struct SegementDescriptor gdt[] = {
	SEG_NULL,
	[SEG_KTEXT] = SEG(0, 0xFFFFFFFF, (STA_X|STA_R), DPL_KERNEL),
	[SEG_KDATA] = SEG(0, 0xFFFFFFFF, STA_W, DPL_KERNEL),
	[SEG_UTEXT] = SEG(0, 0xFFFFFFFF, (STA_X|STA_R), DPL_USER),
	[SEG_UDATA] = SEG(0, 0xFFFFFFFF, STA_W, DPL_USER),
	[SEG_TSS] = SEG_NULL
};

// TSS STACK
uint8_t stack0[1024];

static struct pseudodesc gdt_pd = {
    sizeof(gdt) - 1, (uint32_t)gdt
};

static inline void
lgdt(struct pseudodesc* pd) {
	asm volatile("lgdt (%0);" :: "r"(pd));
	asm volatile("movw %%ax, %%gs;"::"a"(USER_DS));
	asm volatile("movw %%ax, %%fs;"::"a"(USER_DS));
	asm volatile("movw %%ax, %%es;"::"a"(KERNEL_DS));
	asm volatile("movw %%ax, %%ds;"::"a"(KERNEL_DS));
	asm volatile("movw %%ax, %%ss;"::"a"(KERNEL_DS));
	// 转到保护模式执行
	asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

void
gdt_init(void) {
	// 1. 构造GDT
	// 2. 初始化GDT表中的TSS段
	ts.ts_esp0 = (uintptr_t)&stack0 + sizeof(stack0);
	ts.ts_ss0 = KERNEL_DS;

	gdt[SEG_TSS] = SEG16((uint32_t)&ts, sizeof(ts), STS_T32A, DPL_KERNEL);
	gdt[SEG_TSS].sd_s = 0;
	// 2. 加载到GDTR
	lgdt(&gdt_pd);

	// 加载TSS
	ltr(GD_TSS);
}

void
pmm_init(void) {
	gdt_init();
}

#include "kdebug.h"
#include "stdio.h"
#include "x86.h"

#define STACKFRAME_DEPTH 20

void
print_kerninfo(void) {
	extern char etext[], edata[], end[], kern_init[];
	cprintf("------------------------\n");
	cprintf("Kernel Entry Point: 0x%08x \n", kern_init);
	cprintf("etext: 0x%08x \n", etext);
	cprintf("edata: 0x%08x \n", edata);
	cprintf("end: 0x%08x \n", end);
    cprintf("Kernel executable memory footprint: %dKB\n", (end - kern_init + 1023) / 1024);
	cprintf("------------------------\n");
}

static __noinline uint32_t
read_eip(void) {
    uint32_t eip;
    asm volatile("movl 4(%%ebp), %0" : "=r" (eip));
    return eip;
}
static __noinline uint32_t
read_esp(void) {
	uint32_t esp;
	asm volatile(
		"movl %%esp, %0;"	\
		:"=r"(esp):
	);
	return esp;
}

// TODO - 完成输出堆栈的功能
void
print_stackframe(void) {
	uint32_t ebp = read_ebp(), esp = read_esp(), eip = read_eip();
	cprintf("eip: 0x%08x, ebp: 0x%08x, esp: 0x%08x\n", ebp, eip, esp);

	// 要知道堆栈内的内容都是什么，需要构造数据结构，
	// 好知道，调用的函数名字、参数是什么。
	/*
	uint32_t* ptr = (uint32_t*)ptr;
	int i;
	for(i = 0; ptr != NULL && i < STACKFRAME_DEPTH; ++i) {
		ptr = (uint32_t*)ebp;
		cprintf("var%d: 0x%08x\n", i ,*ptr);
		ebp = ebp - 1;
	}
	*/
	cprintf("-------------------------\n");
}

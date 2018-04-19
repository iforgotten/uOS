#include "defs.h"
#include "x86.h"
#include "string.h"
#include "console.h"
#include "kdebug.h"
#include "kmonitor.h"
#include "picirq.h"
#include "trap.h"
#include "intr.h"
#include "pmm.h"
#include "clock.h"

void kern_init(void) __attribute__((noreturn));
void switchTest(void);

void
kern_init(void){
    // BSS（Block Started by Symbol）: 初始化静态变量和全局变量的段
    // 导入两个符号
    // edata == bss的开始地址
    // end == bss的结束地址
    extern char edata[],end[];

    // 初始化BSS为0
    memset((void*)edata, 0, (end-edata));

    // 提供串口，并口，CGA进行显示的驱动程序初始化
    console_init();
    // 输出字符串
    const char* message = "Hello,uOS!";
    cprintf("%s\n\n",message);
    print_kerninfo();
    mon_backtrace(0, NULL, NULL);

    pmm_init();
    // 8259中断设备，中断控制器
    pic_init();
    // 建立中断描述符表
    idt_init();
    // 时钟中断8253
    clock_init();
    // 使能中断
    enable_intr();

    switchTest();

    while(1);
}

void
printCurStatus(void) {
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;
    asm volatile (
            "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));
    cprintf("%d: @ring %d\n", round, reg1 & 3);
    cprintf("%d:  cs = %x\n", round, reg1);
    cprintf("%d:  ds = %x\n", round, reg2);
    cprintf("%d:  es = %x\n", round, reg3);
    cprintf("%d:  ss = %x\n", round, reg4);
    round ++;
}

void
switchToUser(void) {
    asm volatile(
        "subl $0x8, %%esp;"         \
        "int %0;"                   \
        "movl %%ebp, %%esp;"        \
        ::"i"(TRAP_SWITCH_TO_USR)
    );
}

void
switchToKernel(void) {
    asm volatile(
        "int %0;"                   \
        "movl %%ebp, %%esp;"        \
        ::"i"(TRAP_SWITCH_TO_KERN)
    );
}

void 
switchTest(void) {
	cprintf("+++ switch to  user  mode +++\n");
	switchToUser();
    printCurStatus();
    cprintf("+++ switch to  kernel  mode +++\n");
    switchToKernel();
    printCurStatus();
    cprintf("-------------------------------\n");
}

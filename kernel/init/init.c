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
    // 时钟中断8253
    clock_init();
    // 建立中断描述符表
    idt_init();
    // 使能中断
    enable_intr();


    while(1);
}

#include "defs.h"
#include "x86.h"
#include "string.h"
#include "console.h"

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

    while(1);
}

/*
 * pirirq.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#include "picirq.h"
#include "x86.h"

#define IO_PIC1		0x20		// The command port of PIC1
#define IO_PIC2		0xA0		// The command port of PIC2

#define IRQ_OFFSET 	0x20		// 32 ~ 47 为可屏蔽外设中断
#define IRQ_SLAVE	0x2			// PIC1,ICW3的2号端口为从片PIC2

static uint16_t irq_mask = 0xFFFF & ~(1 << IRQ_SLAVE);
static bool is_init = false;

void
pic_setmask(uint16_t mask) {
	irq_mask = mask;
	if(is_init) {
		outb(IO_PIC1+1, mask);
		outb(IO_PIC2+1, mask >> 8);
	}
}

void
pic_init(void) {
	is_init = true;
	// 屏蔽主从8259A的所有中断位
	// 在未初始化完成之前，不进行相应
	outb(IO_PIC1+1, 0xFF);
	outb(IO_PIC2+1, 0xFF);

	// ICW1:
	// D1 = 0时，即SNGL = 0时，有级联
	// D3 = 0, 即LTML= 0，边缘触发方式
	// D4 = 1, 标志位
	outb(IO_PIC1, 0x11);

	// 设置主片的ICW2
	outb(IO_PIC1+1, IRQ_OFFSET);
	// 设置主片的ICW3
	outb(IO_PIC1+1, 1 << IRQ_SLAVE);
	// ICW4:
	// D0: UPM == CPU类型，0 == 8080，1 == 8086
	// D1: AEOI == 指定是否自动终端结束方式,0 == 非自动， 1 == 自动
	// D3: BUF, 0 == 不工作于缓冲，1 == 工作与缓冲
	// D4: SFNM, 决定级联时，是否使用特殊嵌套，0 == 一般嵌套， 1 == 特殊嵌套
	outb(IO_PIC1+1, 0x03);

	// 设置从片PIC2
	outb(IO_PIC2, 0x11);
	outb(IO_PIC2+1, IRQ_OFFSET+8);
	outb(IO_PIC2+1, IRQ_SLAVE);
	outb(IO_PIC2+1, 0x03);

	// OCW1 写入偶地址端口，2、3写入奇地址端口
	// D4、D3位为00时为OCW2，为01时为OCW3
	/*
	 * D0位：RIS，读IRR或ISR选择位：1==读ISR、0==读IRR
	 * D1位：RR:读寄存器：1==允许读IRR、ISR 0==不允许读
	 * D2位：P:查询命令位：1==查询、0==不是查询
	 * D4~D3位：0 1是OCW3的标志
	 * D5位：SMM设置特殊屏蔽方式：1==选择特殊屏蔽方式、0==清除特殊屏蔽方式
	 * D6位：ESMM允许SMM起作用：1==允许、0==不允许
	 */
    outb(IO_PIC1, 0x68);    // clear specific mask
    outb(IO_PIC1, 0x0a);    // read IRR by default

    outb(IO_PIC2, 0x68);
    outb(IO_PIC2, 0x0a);

    if(irq_mask != 0xFFFF) {
    	pic_setmask(irq_mask);
    }
}

void
pic_enable(unsigned int irq) {
	pic_setmask(irq_mask & ~(1 << irq));
}

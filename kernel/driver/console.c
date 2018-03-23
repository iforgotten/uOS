/*
 * console.c
 *
 *  Created on: Mar 23, 2018
 *      Author: alvinli
 */

#include "console.h"
#include "defs.h"
#include "string.h"
#include "x86.h"

// 设备驱动层
// 包括串口，并口，和VGA的操作

#define CGA_TXT_BUFFER		0xB8000
#define CGA_TXT_CONT		0x3D4
#define MDA_TXT_BUFFER		0xB0000
#define MDA_TXT_CONT		0x3B4
#define CRT_ROWS        	25
#define CRT_COLS        	80
#define CRT_SIZE       		(CRT_ROWS * CRT_COLS)


static uint16_t* crtBuff;
static uint16_t crtContoller;
static uint16_t crtPos;

static void CGA_init(void);
static void CGA_putch(int ch);
static void inline CGA_scrollup() __attribute__((always_inline));;
/*
 * CGA:
 * 0xB0000 - 0xB7777 单色字符
 * 0xB8000 - 0xBFFFF 彩色字符
 *
 * I/O 地址：
 * 数据寄存器： 0x3B5/0x3D5
 * 控制寄存器： 0x3B4/0x3D4
 *
 * 存放光标位置的寄存器编号是14和15，共16位
 * 屏幕总共25行，80列
 * 每两个字节表示一个字符，前一个字节是ASCII码，后一个字节表示字符的属性
 */

static void
CGA_init(void) {
	// 初始化CRT的字符缓存， 控制寄存器
	uint16_t* buff = (uint16_t*)CGA_TXT_BUFFER;
	*buff = 0xCCCC;
	if(*buff == 0xCCCC) {
		// 存在CGA区域
		crtBuff = buff;
		crtContoller = CGA_TXT_CONT;
	} else {
		crtBuff = (uint16_t)MDA_TXT_BUFFER;
		crtContoller = MDA_TXT_CONT;
	}

	// 获取CRT光标位置
	uint16_t pos;
	outb(crtContoller, 14);
	pos = inb(crtContoller+1) << 8;
	outb(crtContoller, 15);
	pos = inb(crtContoller+1) | pos;

	crtPos = pos;
}

/*
 * 每个字节占两个字节，前一个字节为ASCII，后一个字节为字符属性
 * 属性为：
 * bit7: Blinking
 * bit6: Background Red
 * bit5: Background Green
 * bit4: Background Blue
 * bit3: Foreground Intensity
 * bit2: Foreground Red
 * bit1: Foreground Green
 * bit0: Foreground Blue
 */
static void
CGA_putch(int ch) {
	// 设置为白底黑字
    if (!(ch & ~0xFF)) {
    	ch |= 0x0700;
    }

    switch(ch & 0xFF) {
    case '\n':
    	crtPos += CRT_COLS - (crtPos % CRT_COLS);
    	break;
    case '\r':
    	crtPos -= (crtPos % CRT_COLS);
    	break;
    case '\t':
    	crtPos += 4;
    	break;
    case '\b':
    	if(crtPos > 0) {
    		crtPos --;
    		crtBuff[crtPos] = 0x0700 | ' ';
    	}
    	break;
    default:
    	crtBuff[crtPos++] = ch;
    	break;
    }

    // scroll up
    if(crtPos >= CRT_SIZE) {
    	memmove((void*)crtBuff, (void*)(CRT_SIZE - CRT_COLS), (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
    	CGA_scrollup();
    }

    // 设置当前的CGA坐标
    outb(crtContoller, 14);
    outb(crtContoller + 1, crtPos >> 8);
    outb(crtContoller, 15);
    outb(crtContoller + 1, crtPos);
}

// 向上卷一行
static void
CGA_scrollup() {
	// MEMMOVE
	int i;
	for(i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; ++i) {
		crtBuff[i] = 0x0700 | ' ';
	}
	crtPos -= CRT_COLS;
}

void
console_init(void) {
	CGA_init();
}

void
console_putch(int ch) {
	CGA_putch(ch);
}


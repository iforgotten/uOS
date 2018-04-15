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

#define COM1				0x3F8
#define COM1_IRQ			4

// The 8 I/O Byte Locations on the UART
// A2 A1 A0
#define COM_RX				0		// Receiver Buffer, read data from RX(DLAB = 0)
#define COM_TX				0		// Transmitter holding, write data to TX(DLAB = 0)
#define COM_DLL				0		// LS Byte(DLAB = 1)
#define COM_DLM				1		// MS Byte(DLAB = 0)
#define COM_LSD				0		// LS Byte of baud rate divisor
#define COM_IER				1		// Interrupt enable Register
#define COM_MSD				1		// MS Byte of baud rate Divisor
#define COM_FCR				2		// FIFO Control Register
#define COM_LCR				3		// Line Control Register
#define COM_MCR				4		// Modem Control Register
#define COM_LSR				5		// Line Status Register, Read-Only.
#define	COM_MS				6		// Modem status

#define COM_LCR_PE_NO		0x08	// PE = 0, no parity
#define COM_LCR_DLAB		0x80	// DL = 1, Enable Divisor latch.
#define COM_LCR_DT_LEN		0x03	// L1, L0 = 11 == 8bits. Data length is 8 bits.

#define COM_FCR_DISABLE		0x0		// disable FIFO

#define COM_LSR_DR			0x01	// RX Read, Data Ready
#define COM_LSR_TH			0x20	// TX Ready.
#define COM_LSR_TE			0x40

#define COM_IER_RDI			0x01	// Enable receiver data interrupt.

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
	*buff = 0x0700;
	if(*buff == 0x0700) {
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
    	memmove((void*)crtBuff, (void*)(CRT_SIZE - CRT_COLS), \
    			(CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
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
	crtPos -= (CRT_ROWS * CRT_COLS);
}

/*
 * Serial Port Settings
 * 1. We need to select speed, number of data bits per character, parity, and number of stop bits per character
 * 2. SPeed:
 * 	  Bit Rats: the sum of transmitting bits per second.
 *    The port speed and device speed must match; 8-N-1: 8 data, no parity bit, and one stop bit.
 * 3. Data bits: the number of data bits in each charactor can be 5, 6, 7, 8 or 9.
 * 4. Parity: None.
 * 5. Stop bits.
 * 6. Conventional notation:The data/parity/stop (D/P/S) conventional notation specifies the framing of a serial connection.
 * 7. Flow control: "handshaking" method. We use software handshaking. XON and XOFF.the direction is from receiver to sender.
 */

/*
 * LS Byte: 000 = 0, write LS Byte of baud rate divisor.
 * MS Byte: 001 = 1
 * FIFO Control Register: 010 = 2
 * LSR(LINE STATUS REGISTER): 101 = 5
 * LCR(LINE CONTROL REGISTER): 011 = 3
 */
static void
serial_init(void)
{
	// 禁止FIFO Buffer
	outb(COM1 + COM_FCR, COM_FCR_DISABLE);

	// 设置Speed,9600bps所需要的DIVISOR == 12D
	// 打开DLAB，然后才能设置Speed
	outb(COM1 + COM_LCR, COM_LCR_DLAB);
	// 采用LSB，即：小端方式存放
	outb(COM1 + COM_DLL, (uint8_t)0x0C);
	outb(COM1 + COM_DLM, 0);

	// 设置8-N-1, 关闭DLAB
	outb(COM1 + COM_LCR, COM_LCR_DT_LEN & ~COM_LCR_DLAB);

	// 关闭 modem controls
	outb(COM1 + COM_MCR, 0);
	// 打开接受中断
	// outb(COM1 + COM_IER, COM_IER_RDI);

	// TODO - 串口的中断服务例程和中断向量还没有添加

}

// TODO - 并口的初始化
void
console_init(void) {
	CGA_init();
	serial_init();
}

// TODO - 完成串口，并口的字符输出
void
console_putch(int ch) {
	CGA_putch(ch);
}


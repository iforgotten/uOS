/*
 * stdio.c
 *
 *  Created on: Mar 23, 2018
 *      Author: alvinli
 */
#include "stdio.h"
#include "console.h"
#include "printfmt.h"

static void
c_putch(int ch, int* cnt) {
	console_putch(ch);
	(*cnt)++;
}

int
_vcprintf(const char* format, va_list argptr) {
	int count;
	count = 0;
	// 处理格式
	__vprintformat((void*)c_putch, &count, format, argptr);
	// 然后进入设备驱动层，调用console向显存写入数据
	return count;
}

int
cprintf(const char* format, ...) {
	va_list ap;
	int count;
	va_start(ap, format);
	count = _vcprintf(format, ap);
	va_end(ap);
	return count;
}

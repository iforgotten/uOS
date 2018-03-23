/*
 * stdio.c
 *
 *  Created on: Mar 23, 2018
 *      Author: alvinli
 */
#include "stdio.h"
#include "console.h"
#include "defs.h"

static void
c_putch(int ch, int* cnt) {
	console_putch(ch);
	(*cnt)++;
}

void
__vprintformat(
		void (*putch)(int,void*),
		void* putData,
		const char* format,
		va_list argptr)
{
	register int ch;
	register char* pArg;
	pArg = NULL;

	// 循环处理
	while(true) {
		// 判断标准格式化字符
		while(((ch = *format) != '%')) {
			format++;
			if(ch == '\0') {
				return;
			}
			putch(ch, putData);
		}

		format++;
		ch = *format;
		format++;
		switch(ch) {
		case 's':
		{
			if((pArg = va_arg(argptr, char*)) == NULL) {
				pArg = "(null)";
			} else {
				while((ch = *pArg) != "\0") {
					pArg++;
					if(ch == '\0') {
						break;
					}
					putch(ch, putData);
				}
			}

		}
			break;
		default:
			break;
		}
	}
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

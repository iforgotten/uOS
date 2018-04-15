/*
 * printfmt.c
 *
 *  Created on: Apr 3, 2018
 *      Author: alvinli
 */

#include "printfmt.h"
// @ap: va list pointer
// @lflag: the data width of data
static unsigned long long
getuint(va_list *ap, int lflag) {
	if(lflag >= 2) {
		return va_arg(*ap, unsigned long long);
	} else if(lflag) {
		return va_arg(*ap, unsigned long);
	} else {
		return va_arg(*ap, unsigned int);
	}
}

static long long
getint(va_list *ap, int lflag) {
	if(lflag > 2) {
		return va_arg(*ap, long long);
	} else if(lflag) {
		return va_arg(*ap, long);
	} else {
		return va_arg(*ap, int);
	}
}
/* *
 * printnum - print a number (base <= 16) in reverse order
 * @putch:       specified putch function, print a single character
 * @putdat:      used by @putch function
 * @num:         the number will be printed
 * @base:        base for print, must be in [1, 16]
 * @width:       maximum number of digits, if the actual width is less than @width, use @padc instead
 * @padc:        character that padded on the left if the actual width is less than @width
 * */
static void
printnum(void (*putch)(int, void*), void *putdat,
        unsigned long long num, unsigned base, int width, int padc) {
    unsigned long long result = num;
    unsigned mod = do_div(result, base);

    if(num >= base) {
    	printnum(putch, putdat, result, base, width-1, padc);
    } else {
    	while(--width > 0)
    		putch(padc,putdat);
    }
    putch("0123456789abcdef"[mod], putdat);
}

/*
 * @putch:		putc function
 * @putData:	the data for the putch
 * @format:		the string.
 * @argptr:		va_list pointer.
 */
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

	int precision = -1, width = -1;
	// 输出数字时，是什么类型
	int base = -1, lflag = 0;
	unsigned long long num = 0;

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

		char padC = ' ';
		lflag = 0;
reswitch:
		format++;
		ch = *format;
		format++;
		switch(ch) {
		case 'c':
			putch(va_arg(argptr,int), putData);
			break;
		case 's':
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
			break;
		case '0':
			padC = '0';
			goto reswitch;
		case '1' ... '9':
			precision = (int)(ch - '0');
			while(true) {
				format++;
				ch = *format;
				precision = precision * 10 + ch - '0';
				if(ch < '0' || ch > '9')
					break;
			}
			goto process_precision;
		case 'd':
			base = 10;
			num = getint(&argptr, lflag);
			goto print_num;
		case 'x':
			base = 16;
			// 根据参数长度，获得数据
			num = getuint(&argptr, lflag);
			// 输出16进制数
		print_num:
			printnum(putch, putData, num, base, width, padC);
			break;
		case '%':
			putch(ch, putData);
			break;

		process_precision:
			if(width < 0) {
				width = precision;
				precision = -1;
			}
			goto reswitch;
		default:
			break;
		}
	}
}

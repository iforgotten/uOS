/*
 * printfmt.h
 *
 *  Created on: Apr 3, 2018
 *      Author: alvinli
 */

#ifndef __LIBS_PRINTFMT_H__
#define __LIBS_PRINTFMT_H__

#include "defs.h"
#include "x86.h"
#include "stdarg.h"

static unsigned long long getuint(va_list *ap, int lflag);
static long long getint(va_list *ap, int lflag);
static void printnum(void (*putch)(int, void*), void *putdat, unsigned long long num, unsigned base, int width, int padc);
void __vprintformat();
#endif /* PRINTFMT_H_ */

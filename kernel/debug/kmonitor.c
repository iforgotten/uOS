/*
 * kmonitor.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#include "kmonitor.h"
#include "kdebug.h"

int
mon_backtrace(int argc, char **argv, struct trapframe *tf) {
	print_stackframe();
	return 0;
}

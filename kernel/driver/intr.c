/*
 * intr.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */


#include "intr.h"
#include "x86.h"

void
enable_intr(void) {
	sti();
}

void
disable_intr(void) {
	cli();
}

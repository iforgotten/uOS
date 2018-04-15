/*
 * trap.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#include "trap.h"

void
idt_init(void) {
	extern uintptr_t __vectors[];
	cprintf("%x", __vectors[0]);
}

void
trap(struct trapframe* tf) {
	return;
}

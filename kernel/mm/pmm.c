/*
 * pmm.c
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */
#include "x86.h"
#include "pmm.h"
#include "memlayout.h"

// TODO - 任务门，及TSS部分
void
gdt_init(void) {

}

void
pmm_init(void) {
	gdt_init();
}

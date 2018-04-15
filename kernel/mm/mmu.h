/*
 * mmu.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_MM_MMU_H__
#define __KERNEL_MM_MMU_H__

#include <defs.h>

#define STA_IG32		0xE				// 32bit Interrupt Gate flag: 01110
#define STA_TG32		0xF				// 32bit Trap Gate flag: 01111

// 80386 Interrupt gate or Trap gate
struct GateDescriptors {
	unsigned gd_offset_15_0 : 16;
	unsigned gd_selector 	: 16;
	unsigned gd_not_used 	: 5;
	unsigned gd_reserved 	: 3;
	unsigned gd_flags 		: 5;
	unsigned gd_dpl 		: 2;
	unsigned gd_p 			: 1;
	unsigned gd_offset_31_16: 16;
};

/*
 * @istrap: 1 == trap gate, 0 == interrupt gate
 * @selector: CS selector
 * @offset: offset in CS selector
 * @dpl:
 * */
#define SETGATE(gate,istrap,selector,offset,dpl) { 		\
	(gate).gd_offset_15_0 = (uint32_t)(offset & 0xffff);				\
	(gate).gd_selector = selector;						\
	(gate).gd_not_used = 0;								\
	(gate).gd_reserved = 0;								\
	(gate).gd_flags = (istrap == 0) ? (STA_IG32) : (STA_TG32);			\
	(gate).gd_dpl = dpl;								\
	(gate).gd_p = 1;									\
	(gate).gd_offset_31_16 = (uint32_t)((offset >> 16) & 0xffff);		\
}

#endif /* __KERNEL_MM_MMU_H__ */

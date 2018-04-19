/*
 * mmu.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_MM_MMU_H__
#define __KERNEL_MM_MMU_H__

#include <defs.h>

// #define STA_IG32		0xE				// 32bit Interrupt Gate flag: 01110
// #define STA_TG32		0xF				// 32bit Trap Gate flag: 01111

// EFLAG REGISTER
// Vol.1 3-15
// Figure 3-8 EFLAGS Register
#define EFL_CF			0x00000000				// Carry Flag
#define EFL_PF			0x00000004				// Parity Flag
#define EFL_AF			0x00000010				// Auxiliary Carry Flag
#define EFL_ZF			0x00000040				// Zero Flag
#define EFL_SF			0x00000080				// Sign Flag
#define EFL_TF			0x00000100				// Trap Flag
#define EFL_IF			0x00000200				// Interrupt Enable Flag
#define EFL_DF			0x00000400				// Direction Flag
#define EFL_OF			0x00000800				// Overflow Flag
#define EFL_IOPL		0x00003000				// IO Privilege Level
#define EFL_NT			0x00004000				// Nested Task
#define EFL_RF			0x00010000				// Resume Flag
#define EFL_VM			0x00020000				// Virtual-8086 Mode
#define EFL_AC			0x00040000				// Alignment Check / Access Control
#define EFL_VIF			0x00080000				// virtual Interrupt Flag
#define EFL_VIP			0x00100000				// Virtual Interrupt Pending
#define EFL_ID			0x00200000				// ID Flag

#define EFL_IOPL_0		0
#define EFL_IOPL_1		1
#define EFL_IOPL_2		2
#define EFL_IOPL_3		3

/*
	If the size of a stack segment needs to be changed dynamically, 
	the stack segment can be an expand-down data segment (expansion-
	direction flag set)

	Code segments can be execute-only or execute/read, depending on the 
	bit. An execute/read segment might be used when constants or other 
	static datsetting of the read-enablea have been placed with instruction
	code in a ROM.
*/

// 3.4.5.1 Code- and Data-Segment Descriptor Types
// Table 3-1.Code- and Data-Segment Types
#define STA_A			0x1				// Read-Only, accessed
#define STA_W			0x2				// Writable. In protected mode, code segments are not writable
#define STA_R			0x2				// Readable. non-executable	
#define STA_X			0x8				// Execute-Only
#define STA_C			0x4				// Conforming Segment.(Only Excutable)
#define STA_E			0x4				// Read-Only, expand-down(Only Data-Segment)

// Table 3-2. System-Segment and Gate-Descriptor Types
#define STS_T16A		0x1
#define STS_LDT			0x2
#define STS_T16B		0x3
#define STS_CG16		0x4
#define STS_TG			0x5
#define STS_IG16		0x6
#define STS_TG16		0x7
#define STS_T32A		0x9
#define STS_T32B		0xB
#define STS_CG32		0xC
#define STS_IG32		0xE
#define STS_TG32		0xF

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
	(gate).gd_flags = (istrap == 0) ? (STS_IG32) : (STS_TG32);			\
	(gate).gd_dpl = dpl;								\
	(gate).gd_p = 1;									\
	(gate).gd_offset_31_16 = (uint32_t)((offset >> 16) & 0xffff);		\
}

// Vol3A. 5.8.3 Call Gate
// Figure 5-9. Call-Gate Descriptor in IA-32e Mode
/*
	@gate
	@ss: segment selector
	@offset: offset in segment
	@dpl: Descriptor Privilege Level
	the P bit. is Gate Valid
*/
#define SETCALLSTATE(gate,ss,offset,dpl) {				\
	(gate).gd_off_15_0 = (offset & 0xFFFF);				\
	(gate).gd_selector = ss;							\
	(gate).gd_not_used = 0;								\
	(gate).gd_reserved = 0;								\
	(gate).gd_flags = STS_CG32;							\
	(gate).gd_dpl = dpl;								\
	(gate).gd_p = 1;									\
	(gate).gd_off_31_16 = ((offset >> 16) & 0xFFFF);	\
}				

// Segement Descriptors
// Vol.3 3-9
// Figure 3-8. Segement Descriptor
struct SegementDescriptor {
	unsigned sd_low_segment_limit_15_0		: 16;
	unsigned sd_low_base_address_15_0		: 16;
	unsigned sd_high_base_23_16				: 8;
	unsigned sd_type						: 4;
	unsigned sd_s 							: 1;			// Descriptor type (0 = system; 1 = code or data)
	unsigned sd_dpl							: 2;			// Descriptor privilege level
	unsigned sd_p 							: 1;			// Segment present
	unsigned sd_high_seg_limit_19_16		: 4;
	unsigned sd_avl							: 1;			// Available for use by system software
	unsigned sd_l 							: 1;			// 64-bit code segment (IA-32e mode only)
	unsigned sd_D_or_B						: 1;			// Default operation size (0 = 16-bit segment; 1 = 32-bit segment)
	unsigned sd_g 							: 1;
	unsigned sd_high_base_31_24				: 8;
};

#define SEG_NULL										\
(struct SegementDescriptor){ 0,0,0,0,0,0,0,0,0,0,0,0,0 }

#define SEG16(base,limit,type,dpl)						\
(struct SegementDescriptor){							\
	(limit & 0xFFFF), (base & 0xFFFF), ((base >> 16) & 0xFF),	\
	(type & 0xF), 1, (dpl & 0x03), 1, ((limit >> 16) & 0xF),	\
	0, 0, 1, 0, ((base >> 24) & 0xFF)					\
}

#define SEG(base,limit,type,dpl)						\
(struct SegementDescriptor){							\
	(limit & 0xFFFF), (base & 0xFFFF), ((base >> 16) & 0xFF),	\
	(type & 0xF), 1, (dpl & 0x03), 1, ((limit >> 16) & 0xF),	\
	0, 0, 1, 1, ((base >> 24) & 0xFF)					\
}

// Figure 7.2. 32-Bit Task-State Segment (TSS)
// Vol.3A. 7-3
// Reserved bits. set to 0.
struct TaskState {
	uint32_t	ts_pre_tlink;				// Previous Task Link
	uintptr_t	ts_esp0;					// stack pointers and segment selectors
	uint16_t	ts_ss0;						// segment descriptor
	uint16_t	ts_padding1;				
	uintptr_t	ts_esp1;
	uint16_t	ts_ss1;
	uint16_t	ts_padding2;
	uintptr_t	ts_esp2;
	uint16_t	ts_ss2;
	uint16_t	ts_padding3;
	uintptr_t	ts_cr3;						// page directory base
	uintptr_t	ts_eip;
	uint32_t	ts_eflags;
	uint32_t	ts_eax;
	uint32_t	ts_ecx;
	uint32_t	ts_edx;
	uint32_t	ts_ebx;
	uintptr_t	ts_esp;
	uintptr_t 	ts_ebp;
	uint32_t	ts_esi;
	uint32_t	ts_edi;
	uint16_t	ts_es;
	uint16_t	ts_padding4;
	uint16_t	ts_cs;
	uint16_t	ts_padding5;
	uint16_t	ts_ss;
	uint16_t	ts_padding6;
	uint16_t	ts_ds;
	uint16_t	ts_padding7;
	uint16_t	ts_fs;
	uint16_t	ts_padding8;
	uint16_t	ts_gs;
	uint16_t	ts_padding9;
	uint16_t	ts_ldt;
	uint16_t	ts_padding10;
	uint16_t	ts_t;						// trap
	uint16_t	ts_IOMB;					// IO Map Base Address
};
#endif /* __KERNEL_MM_MMU_H__ */

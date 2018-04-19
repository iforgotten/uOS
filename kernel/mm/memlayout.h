/*
 * memlayout.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_MM_MEMLAYOUT_H__
#define __KERNEL_MM_MEMLAYOUT_H__

// DPL
#define DPL_USER			(0x3)
#define DPL_KERNEL			(0x0)

// 全局段号
#define SEG_KTEXT			1
#define SEG_KDATA			2
#define SEG_UTEXT			3
#define SEG_UDATA			4
#define SEG_TSS				5

// 全局描述符号
#define GD_KTEXT    ((SEG_KTEXT) << 3)        // kernel text
#define GD_KDATA    ((SEG_KDATA) << 3)        // kernel data
#define GD_UTEXT    ((SEG_UTEXT) << 3)        // user text
#define GD_UDATA    ((SEG_UDATA) << 3)        // user data
#define GD_TSS        ((SEG_TSS) << 3)        // task segment selector

#define KERNEL_CS    ((GD_KTEXT) | DPL_KERNEL)
#define KERNEL_DS    ((GD_KDATA) | DPL_KERNEL)
#define USER_CS        ((GD_UTEXT) | DPL_USER)
#define USER_DS        ((GD_UDATA) | DPL_USER)

#endif /* __KERNEL_MM_MEMLAYOUT_H__ */

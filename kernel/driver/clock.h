/*
 * clock.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_DRIVER_CLOCK_H__
#define __KERNEL_DRIVER_CLOCK_H__

#include "defs.h"

extern volatile size_t ticks;
void clock_init(void);

#endif /* __KERNEL_DRIVER_CLOCK_H__ */

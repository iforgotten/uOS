/*
 * kmonitor.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_DEBUG_KMONITOR_H__
#define __KERNEL_DEBUG_KMONITOR_H__

#include "trap.h"

int mon_backtrace(int argc, char **argv, struct trapframe *tf);

#endif /* __KERNEL_DEBUG_KMONITOR_H__ */

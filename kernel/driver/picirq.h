/*
 * picirq.h
 *
 *  Created on: Apr 15, 2018
 *      Author: alvinli
 */

#ifndef __KERNEL_DRIVER_PICIRQ_H__
#define __KERNEL_DRIVER_PICIRQ_H__

void pic_init(void);
void pic_enable(unsigned int irq);

#endif /* __KERNEL_DRIVER_PICIRQ_H__ */

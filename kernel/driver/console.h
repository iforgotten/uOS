/*
 * console.h
 *
 *  Created on: Mar 23, 2018
 *      Author: alvinli
 */

#ifndef __KERN_DRIVER_CONSOLE_H__
#define __KERN_DRIVER_CONSOLE_H__

// 提供显示的驱动程序
// 并口，串口， CGA
void console_init(void);
void console_putch(int ch);

#endif /* CONSOLE_H_ */

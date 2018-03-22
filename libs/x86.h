//
// Created by alvinli on 18-3-20.
//

#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include "../libs/defs.h"

static inline uint8_t inb(uint16_t port) __attribute__((always_inline));
static inline void outb(uint16_t port, uint8_t data) __attribute__((always_inline));
static inline void outw(uint16_t port, uint16_t data) __attribute__((always_inline));
static inline void insl(uint32_t port, void *addr, int cnt) __attribute__((always_inline));

static inline uint8_t
inb(uint16_t port) {
    uint8_t data;
    asm volatile (
        "inb %1, %0"
        : "=a" (data)
        : "d" (port)
    );
    return data;
}

static inline void
outb(uint16_t port, uint8_t data) {
    asm volatile (
        "outb %0, %1"
        :: "a" (data), "d" (port)
    );
}

static inline void
outw(uint16_t port, uint16_t data) {
    asm volatile (
        "outw %0, %1"
        :: "a" (data), "d" (port)
    );
}

static inline void
insl(uint32_t port, void *addr, int cnt) {
    // 串处理指令
    // http://blog.sina.com.cn/s/blog_b1e011040101gs09.html
    // 把段口号在DX寄存器中的I/O空间的字、字节、双字
    // 传送到附加段中的由目的变址寄存器所指向的存储单元中
    // 并根据DF的值和数据类型修改目的变址寄存器的内容。
    /*
     *  mov edi, addr
     *                                 mov     ecx, cnt
        .text:00007CCA                 mov     edx, port
        .text:00007CCF                 cld
        .text:00007CD0                 repne insd
        .text:00007CD2                 pop     edi
        .text:00007CD3                 pop     ebp
        .text:00007CD4                 retn
     */

    asm volatile(
        "cld;"      \
        "rep insl;"   \
        : "+D"(addr),"+c"(cnt)
        : "d"(port)
        : "memory"
    );
}

#endif /* !__LIBS_X86_H__ */
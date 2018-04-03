//
// Created by alvinli on 18-3-20.
//

#ifndef __LIBS_X86_H__
#define __LIBS_X86_H__

#include "defs.h"

#define do_div(n, base) ({                                        	\
    unsigned long __upper, __low, __high, __mod, __base;        	\
    __base = (base);                                            	\
    asm("" : "=a" (__low), "=d" (__high) : "A" (n));            	\
    __upper = __high;                                            	\
    if (__high != 0) {                                            	\
        __upper = __high % __base;                                	\
        __high = __high / __base;                                	\
    }                                                            	\
    asm("divl %2" : "=a" (__low), "=d" (__mod)                   	\
        : "rm" (__base), "0" (__low), "1" (__upper));            	\
    asm("" : "=A" (n) : "a" (__low), "d" (__high));                	\
    __mod;                                                        	\
 })


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


static inline void* __memset(void* ptr, char c, size_t cnt) __attribute__((always_inline));
static inline void* __memmove(void* dst, void* src, size_t cnt) __attribute__((always_inline));

#ifndef __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_MEMSET
static inline void*
__memset(void *ptr, char c, size_t cnt) {
    asm volatile(
		"cld;"      \
		"rep stosb;"
		: "+D"(ptr),"+c"(cnt)
		: "a"(c)
		: "memory"
    );
    return ptr;
}
#endif

#ifndef __HAVE_ARCH_MEMMOVE
#define	__HAVE_ARCH_MEMMOVE

// 从@src处，移动@cnt个字节到@dst处
static inline void*
__memmove(void* dst, void* src, size_t cnt) {
	void* ret = dst;
	asm volatile(
		"cld;"		\
		"rep movsb;"
		: "+D"(dst),"+S"(src),"+c"(cnt)
		::"memory"
	);
	return ret;
}
#endif


#endif /* !__LIBS_X86_H__ */

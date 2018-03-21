#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#define __always_inline inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))

// 定义布尔类型
typedef int bool;

// 明确基本类型占多少位
typedef char                int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

// 指针类型
typedef int32_t     intptr_t;
// 地址数值的类型
typedef uint32_t    uintptr_t;

// 内存大小
typedef uintptr_t size_t;


#endif /* !__LIBS_DEFS_H__ */


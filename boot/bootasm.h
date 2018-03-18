#ifndef __BOOTASM_H__
#define __BOOTASM_H__

// GDT表中第一项为空项，不能被CPU读取
#define SEG_NULL				\
	.word	0, 0;				\
	.byte	0, 0, 0, 0

// 正常段
// 段描述符有8个字节
// 段基址、段界限和类型
// 内存的顺序为：从低到高

#define SEG_NORMAL(base, limit, type)								\
	.word ((limit) & 0xFFFF), ((base) & 0xFFFF);					\
	.byte (((base) >> 16) & 0xFF), (0x90 | type),					\
		(0xC0 | (((limit) >> 16) & 0xFF)), (((base) >> 24) & 0xFF)



// 描述符表项中的TYPE位
// 需要表述的有：可读、可写、可执行
#define SEG_TYPE_X       0x8     // 只执行
#define SEG_TYPE_E       0x4     // 只读，向下扩展
#define SEG_TYPE_W       0x2     // 写
#define SEG_TYPE_R       0x2     // 读
#define SEG_TYPE_A       0x1     // 只读，已访问

#endif /* !__BOOTASM_H__*/

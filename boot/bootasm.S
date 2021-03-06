#include "bootasm.h"

.set CR0_PE_ENABLE,			0x1
.set CSEG_PRT_DESCRIPOR,	0x8
.set DSEG_PRT_DESCRIPOR,	0x10

# 从实模式到保护模式的主要流程
# 1. 准备GDT
# 2. 装载GDTR
# 3. 使能A20
# 4. 使能PE
# 5. 跳转到保护模式下执行

.text
.global start
.code16
    jmpl $0x0, $start
start:
	# 初始化除堆栈段以外的段寄存器，为加载GDTR做准备
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es

	# 关闭中断
	cli
	# 开启A20地址线
	# 最简单的方法是给0x92的第1位置1
	# 但是某些情况下会导致一些问题
	# inb	$0x92, %al
	# orb	$0x2, %al
	# outb	%al, $0x92

	# 现采用向8042芯片的0x64端口的第1位置1
setA20.1:
	inb		$0x64, %al
	testb	$0x02, %al
	jnz		setA20.1
	
	movb	$0xd1, %al
	outb	%al, $0x64 

setA20.2:
	inb		$0x64, %al
	testb	$0x02, %al
	jnz		setA20.2

	# 参数经过0x60传递，0xdf的意思是说打开A20
	movb	$0xdf, %al
	outb	%al, $0x60

	# 初始化GDTR寄存器
	lgdt	GDTDESC

	# 读取CR0寄存器，使能保护PE
	movl	%cr0, %eax
	orl		$CR0_PE_ENABLE, %eax
	movl	%eax, %cr0

	# 跳转到32位段选择子处开始执行
	ljmp 	$CSEG_PRT_DESCRIPOR, $code32CS

.code32
code32CS:
	# 设置保护模式下的各个段选择子
	movw	$DSEG_PRT_DESCRIPOR, %ax
	movw	%ax, %ds
	movw	%ax, %ss
	movw	%ax, %es
	movw	%ax, %gs
	movw	%ax, %fs

	# 设置堆栈栈顶指针esp和栈底指针ebp
	# 0x7C00之前的内存空间并没有用到
	# 可以做为bootloader的堆栈进行使用
	movl	$0x0, %ebp
	movl	$start, %esp
	call	bootmain

errWait:
	# 不应该跳转到这里，如果跳转到这里就是出现错误了
	jmp		errWait


# 强制4字节对齐
.p2align 2
GDT:
	SEG_NULL													# 段选择子为0，即：index == 0
	SEG_NORMAL(0x0, 0xffffffff, SEG_TYPE_X | SEG_TYPE_R)		# 代码段，段选择子为0x8，即：index == 0x8
	SEG_NORMAL(0x0, 0xffffffff, SEG_TYPE_W)						# 数据段，段选择子位0x10, 即：index == 0x10

# GDTR共6个字节（48bit），高32位为基地址，后16位为段界限
# 遵循C调用约定，调用者负责清理堆栈，参数从右向左传递
GDTDESC:
	.word	(GDTDESC - GDT - 1)		# sizeof(GDT) - 1 个字节 == 3 * 8 - 1 = 23个字节，化为16进制为 == 0x17
	.long	GDT



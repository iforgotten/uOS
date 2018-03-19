TARGETS		:= tools/sign obj/bootblock.out obj/bootblock

CPFLAGS		:= -S -O binary
LDFLAGS		:= -N -m elf_i386 -e start -Ttext 0x7C00 -o
DPFLAGS		:= -S
CCFLAGS		:= -fno-builtin -Wall -MD -ggdb -m32 -fno-stack-protector -nostdinc -Iinclude -Iinclude/x86

CP			:= objcopy
LD			:= ld
DP			:= objdump
CC			:= gcc 

all:$(TARGETS) 
obj/bootblock.out: obj/bootblock.o
	$(CP) $(CPFLAGS) $^ $@
obj/bootblock.o: obj/bootasm.o
	$(LD) $(LDFLAGS) $@ $^
	$(DP) $(DPFLAGS) $@ > obj/bootblock.asm
obj/bootasm.o: boot/bootasm.S
	mkdir obj
	$(CC) $(CCFLAGS) -c $^ -o $@

tools/sign: tools/sign.c
	$(CC) -O2 -o $@ $^
	
obj/bootblock: tools/sign
	tools/sign obj/bootblock.out obj/bootblock
	# dd if=/dev/zero of=obj/uOS.img count=10000
	dd if=obj/bootblock of=obj/uOS.img count=512 conv=notrunc

.PHONY:clean qemu-mon rebuild 
clean:
	rm -rf obj
	rm -f tools/sign

qemu-mon:
	gnome-terminal -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -fda obj/uOS.img"
	sleep 1
	gnome-terminal -e "gdb -q -x tools/gdbinit"
rebuild: clean all

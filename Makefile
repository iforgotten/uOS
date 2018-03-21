TARGETS		:= bin/kernel tools/sign obj/bootmain.o  obj/bootblock.out obj/bootblock

CPFLAGS		:= -S -O binary
LDFLAGS		:= -N -m elf_i386 -e start -Ttext 0x7C00 -o
DPFLAGS		:= -S
CCFLAGS		:= -fno-builtin -Wall -MD -ggdb -m32 -fno-stack-protector -nostdinc -Iinclude -Iinclude/x86

CP			:= objcopy
LD			:= ld
DP			:= objdump
CC			:= gcc 

all:$(TARGETS) 
bin/kernel: kernel/init/init.o
	$(LD) -m elf_i386 -Ttext 0x100000 -e kern_init -o bin/kernel kernel/init/init.o
kernel/init/init.o:kernel/init/init.c
	$(CC) $(CCFLAGS) -c $^ -o $@

obj/bootblock.out: obj/bootblock.o
	$(CP) $(CPFLAGS) $^ $@
obj/bootblock.o: obj/bootasm.o obj/bootmain.o
	$(LD) $(LDFLAGS) $@ $^
	$(DP) $(DPFLAGS) $@ > obj/bootblock.asm
obj/bootmain.o: boot/bootmain.c
	mkdir obj
	$(CC) $(CCFLAGS) -Os -c $^ -o $@ 
obj/bootasm.o: boot/bootasm.S
	$(CC) $(CCFLAGS) -Os -c $^ -o $@

tools/sign: tools/sign.c
	$(CC) -O2 -o $@ $^
	
obj/bootblock: tools/sign
	tools/sign obj/bootblock.out obj/bootblock
	dd if=/dev/zero of=obj/uOS.img count=10000
	dd if=obj/bootblock of=obj/uOS.img count=512 conv=notrunc
	dd if=bin/kernel of=obj/uOS.img seek=1 conv=notrunc

.PHONY:clean debug qemu-mon bios-mon rebuild 
clean:
	rm -rf obj
	rm -f tools/sign
	rm -f bin/kernel kernel/init/init.o kernel/init/init.d
qemu-mon:
	gnome-terminal -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda obj/uOS.img"
	sleep 1
	gnome-terminal -e "gdb -q -x tools/bootinit"

bios-mon:
	gnome-terminal -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda obj/uOS.img"
	sleep 1
	gnome-terminal -e "gdb -q -x tools/biosinit"
debug:
	gnome-terminal -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda obj/uOS.img"
	sleep 1
	gnome-terminal -e "gdb -q -x tools/gdbinit"
rebuild: clean all

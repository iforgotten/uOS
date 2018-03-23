TARGETS		:= tools/sign obj/bootmain.o obj/bootblock.out bin/bootblock bin/kernel bin/uOS.img kernel/driver/console.o

CPFLAGS		:= -S -O binary
LDFLAGS		:= -m elf_i386 -N
LDLABLE		:= -e
LDTEXT		:= -Ttext
DPFLAGS		:= -S
LIBSDIR		:= /home/alvinli/workspace/uOS/libs
KERNLIBSDIR += /home/alvinli/workspace/uOS/kernel/libs 		\
			   -I/home/alvinli/workspace/uOS/kernel/driver

CFLAGS		:= -fno-builtin -Wall -MD -fno-stack-protector -nostdinc -I$(LIBSDIR) -I$(KERNLIBSDIR)

CP		:= objcopy
LD		:= ld 
DP		:= objdump
CC		:= gcc -ggdb -m32

V		:= @
QEMU 	:= qemu
SEMICOLON	:= /
TERMINAL	:= gnome-terminal
TOOLSRC		:= $(wildcard tools/*.c)
TOOLSELF	:= tools/sign
BINDIR		:= bin
# functions
make_dir = $(shell mkdir -p $(1))
totarget = $(addprefix $(BINDIR)$(SEMICOLON),$(1))

OBJDIR 		:= $(call make_dir, obj)
$(call make_dir, bin)

KERNEL		= $(call totarget,kernel)
BOOTBLOCK 	= $(call totarget,bootblock)
UOSIMG		= $(call totarget,uOS.img)

all:$(TARGETS) 
# 生成内核
$(KERNEL): kernel/init/init.o libs/string.o kernel/driver/console.o kernel/libs/stdio.o
	$(LD) $(LDFLAGS) $(LDLABLE) kern_init $(LDTEXT) 0x100000 -T tools/kernel.ld -o $@ $^
kernel/init/init.o:kernel/init/init.c 
	@echo "=========="
	@echo "KERNEL"
	@echo "=========="
	$(CC) $(CFLAGS) -c $^ -o $@
libs/string.o:libs/string.c 
	$(CC) $(CFLAGS) -c $^ -o $@
kernel/driver/console.o: kernel/driver/console.c
	$(CC) $(CFLAGS) -c $^ -o $@
kernel/libs/stdio.o:kernel/libs/stdio.c
	$(CC) $(CFLAGS) -c $^ -o $@

# 生成bootblock.out
obj/bootblock.out: obj/bootblock.o
	$(CP) $(CPFLAGS) $^ $@
obj/bootblock.o: obj/bootasm.o obj/bootmain.o
	$(LD) $(LDFLAGS) $(LDLABLE) start $(LDTEXT) 0x7C00 -o $@ $^
	$(DP) $(DPFLAGS) $@ > obj/bootblock.asm
obj/bootmain.o: boot/bootmain.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@ 
obj/bootasm.o: boot/bootasm.S
	@echo "=========="
	@echo "BOOTLOADER"
	@echo "=========="
	$(CC) $(CFLAGS) -Os -c $^ -o $@

# 创建sign
$(TOOLSELF):$(TOOLSRC)
	$(CC) -O2 -o $@ $^

# 签名bootloader
# 
$(BOOTBLOCK): $(TOOLSELF)
	@echo "=========="
	@echo "SIGN BOOTLOADER"
	@echo "=========="
	$^ obj/bootblock.out $@

# 生成uOS硬盘镜像
$(UOSIMG): $(BOOTBLOCK) $(KERNEL)
	@echo "=========="
	@echo "UOSIMG"
	@echo "=========="
	dd if=/dev/zero of=$@ count=10000
	dd if=$(BOOTBLOCK) of=$@ count=512 conv=notrunc
	dd if=$(KERNEL) of=$@ seek=1 conv=notrunc


.PHONY:clean gdb debug qemu-kern qemu-mon bios-mon rebuild 
clean:
	rm -rf obj bin
	rm -f tools/*.o tools/*.d tools/sign
	rm -f kernel/init/*.o kernel/init/*.d
	rm -f kernel/libs/*.o kernel/libs/*.d
	rm -f kernel/driver/*.o kernel/driver/*.d
	rm -f libs/*.o libs/*.d
qemu-mon:
	$(V) $(TERMINAL) -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda bin/uOS.img"
	$(V) sleep 1
	$(V) $(TERMINAL) -e "gdb -q -x tools/bootinit"
bios-mon:
	$(V) $(TERMINAL) -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda bin/uOS.img"
	$(V) sleep 1
	$(V) $(TERMINAL) -e "gdb -q -x tools/biosinit"
debug:
	$(V) $(TERMINAL) -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda bin/uOS.img"
	$(V) sleep 1
	$(V) $(TERMINAL) -e "gdb -q -x tools/gdbinit"
qemu-kern: 
	$(V) $(TERMINAL) -e "qemu -S -s -d in_asm -D obj/q.log -monitor stdio -hda bin/uOS.img"
	$(V) sleep 1
	$(V) $(TERMINAL) -e "gdb -q -x tools/kerninit"
gdb:
	$(V)$(QEMU) -S -s -parallel stdio -hda bin/uOS.img -serial null
rebuild: clean all

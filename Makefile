PWD 	:= $(shell pwd)

CC		:= gcc -ggdb
CP		:= objcopy
LD		:= ld 
DP		:= objdump

CPFLAGS		:= -S -O binary
LDFLAGS		:= -m elf_i386 -N
LDLABLE		:= -e
LDTEXT		:= -Ttext
DPFLAGS		:= -S

CFLAGS		:= -fno-builtin -Wall -fno-stack-protector -nostdinc -MD -m32
V			:= @
QEMU 		:= qemu
TERMINAL	:= gnome-terminal

RM			:= rm -f 
SEMI		:= /


# 最后在编译之前生成bin和obj文件夹
ifndef OBJDIR
OBJDIR	:= $(shell if find obj -type d 2>&1 | grep 'No' > /dev/null 2>&1; \
			then mkdir -p 'obj';\
			echo 'obj';\
			else echo 'obj';fi)
endif

ifndef BINDIR
BINDIR	:= $(shell if find bin -type d 2>&1 | grep 'No' > /dev/null 2>&1; \
			then mkdir -p 'bin';\
			echo 'bin';\
			else echo 'bin';fi)
endif
# --------------------------------------------------------------------------------------------------
# Create Bootblock
BOOT_DIR	:= boot/
BOOT_ELF	:= $(BINDIR)$(SEMI)bootblock
BOOT_OUT	:= $(OBJDIR)$(SEMI)bootblock.out
BOOT_INCLUDE := libs

BOOT_OUT_OBJS := $(patsubst %.out, %.o, $(BOOT_OUT))
BOOT_SOURCE	:= $(wildcard $(BOOT_DIR)*.c $(BOOT_DIR)*.S)
BOOT_INCLUDE := $(addprefix $(PWD)$(SEMI), $(BOOT_INCLUDE))
BINCLUDE 	:= $(addprefix -I, $(BOOT_INCLUDE))

BOOT_OBJS	:= $(addsuffix .o, $(basename $(BOOT_SOURCE)))
BOOT_TO_OBJECT = $(addprefix $(OBJDIR)$(SEMI),$(notdir $(addsuffix .o, $(basename $(1)))))
BOOT_OBJS = $(OBJDIR)$(SEMI)bootasm.o $(OBJDIR)$(SEMI)bootmain.o

define CREATE_BOOT_OBJ_TARGET
$(call BOOT_TO_OBJECT,$(1)) : $(1)
	$(V) $(CC) $(CFLAGS) $(BINCLUDE) -Os -c $$^ -o $$@ 
endef

# --------------------------------------------------------------------------------------------------
# Create tools/sign
TOOLS_EXCLUDE := tools/vector.c
TOOLSRC		:= $(filter-out $(TOOLS_EXCLUDE),$(wildcard tools/*.c))
TOOLSELF	:= tools/sign
BOOT_BLOCK	:= $(BINDIR)$(SEMI)bootblock
#----------------------------------------------------------------------------------------------------
# Create Kernel
KERN_DIRS		= $(shell find kernel/ -maxdepth 3 -type d)
KERN_DIRS 		+= $(shell find libs/ -maxdepth 3 -type d)
KERN_SOURCE		+= $(foreach dir, $(KERN_DIRS), $(wildcard $(dir)/*.c) $(wildcard $(dir)/*.S))
KERN_INCLUDE 	+= 	libs			\
					kernel/libs		\
					kernel/driver	\
					kernel/debug	\
					kernel/trap		\
					kernel/mm		\

KERN_INCLUDE := $(addprefix $(PWD)$(SEMI), $(KERN_INCLUDE))
KERN_OBJS	:= $(addsuffix .o, $(basename $(KERN_SOURCE)))
KINCLUDE 	:= $(addprefix -I, $(KERN_INCLUDE))
KERNEL 	:= $(BINDIR)$(SEMI)kernel
KERN_LD := tools/kernel.ld

# file.c
KERN_TO_OBJECT = $(addprefix $(OBJDIR)$(SEMI),$(notdir $(addsuffix .o, $(basename $(1)))))
KERN_OBJS = $(foreach src,$(KERN_SOURCE),$(call KERN_TO_OBJECT,$(src)))

define CREATE_KERN_OBJ_TARGET
$(call KERN_TO_OBJECT,$(1)) : $(1)
	$(V) $(CC) $(CFLAGS) $(KINCLUDE) -MD -c -o $$@ $$^
endef
# ---------------------------------------------------------------------------------------------------
# 生成uOS.img
UOSIMG = $(BINDIR)$(SEMI)uOS.img
#---------------------------------------------------------------------------------------------------
# 生成内核
.PHONY:all clean gdb debug qemu-kern qemu-mon bios-mon 
all: $(BOOTBLOCK) $(TOOLSELF) $(KERNEL) $(BOOT_OUT) $(UOSIMG)
$(KERNEL) : $(KERN_OBJS)
	@echo "================="
	@echo "Create Kernel"
	@echo "================="
	$(V) $(LD) $(LDFLAGS) $(KINCLUDE) $(LDLABLE) kern_init $(LDTEXT) 0x100000 -T $(KERN_LD) -o $@ $^

$(foreach src,$(KERN_SOURCE),$(eval $(call CREATE_KERN_OBJ_TARGET,$(src))))

$(BOOT_OUT) : $(BOOT_OUT_OBJS)
	@echo "================="
	@echo "Create Bootblock.out"
	@echo "================="
	$(CP) $(CPFLAGS) $^ $@
$(BOOT_OUT_OBJS) : $(BOOT_OBJS)
	@echo $(BOOT_OBJS)
	$(LD) $(LDFLAGS) $(LDLABLE) start $(LDTEXT) 0x7C00 -o $@ $^
	$(DP) $(DPFLAGS) $@ > $(patsubst %.o, %.asm, $(BOOT_OUT_OBJS))

$(foreach src,$(BOOT_SOURCE),$(eval $(call CREATE_BOOT_OBJ_TARGET,$(src))))

# 签名bootloader
$(BOOT_BLOCK) : $(TOOLSELF)
	@echo "=========="
	@echo "SIGN BOOTLOADER"
	@echo "=========="
	$(V) $^ $(BOOT_OUT) $@

# 创建sign
$(TOOLSELF) : $(TOOLSRC)
	@echo "=========="
	@echo "Create Sign"
	@echo "=========="
	$(V) $(CC) -O2 -o $@ $^

# 生成uOS硬盘镜像
$(UOSIMG): $(BOOT_BLOCK) $(KERNEL)
	@echo "=========="
	@echo "UOSIMG"
	@echo "=========="
	$(V) dd if=/dev/zero of=$@ count=10000
	$(V) dd if=$(BOOT_BLOCK) of=$@ count=512 conv=notrunc
	$(V) dd if=$(KERNEL) of=$@ seek=1 conv=notrunc

clean:
	rm -r $(OBJDIR) $(BINDIR)
	rm -f $(TOOLSELF)
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
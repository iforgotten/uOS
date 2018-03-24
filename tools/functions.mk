OBJPREFIX := obj
BINPREFIX := bin

filelist = $(filter $(if $(2), $(addprefix %., $(2)), %),\
			 $(wildcard $(addprefix $(SEMI)*, $(1))))

# change $(name) to $(OBJDIR)$(name)
packetname = $(if $(1),$(addprefix $(OBJPREFIX)$(SEMI),$(1)),$(OBJPREFIX))

# 获取.obj文件
# 如果$(1)不空，则添加前缀，否则就是本身
# $(1): files  $(2): dir
# e.g. dir/files.o
to_obj = $(addsuffix .o, $(basename $(if $(2), $(addprefix $(2)$(SEMI),$(1)),$(1))))

# 获取.d依赖文件
# $1: files $2: dir
# e.g. dir/files.d
to_dep = $(patsubst %.o, %.d, $(call to_obj, $(1), $(2)))

# 构造目标文件，将可执行文件放到bin文件夹
# $(1): file
to_target = $(addprefix $(BINPREFIX)$(SEMI), $(1))

# 生成dependency
# file, CC, flags, dir
define ccTemplate
$$(call to_dep, $(1), $(4)): $(1) 
	@$(2) -I$$(dir $(1)) $(3) --MM $$< -MT "$$(patsubst %.d,%.o,$$@) $$@"> $$@
$$(call to_obj, $(1), $(4)): $(1) 
	@echo + CC $$<
	$(V)$(2) -I$$(dir $(1)) $(3) -c $$< -o $$@
ALLOBJS += $$(call to_obj,$(1),$(4))
endef


# files, output, cc, flags, dir
define doAddFiles
__temp_packet__ := $(call packetname, $(1))
__temp_objs__ := $(call to_obj, $(1), $(5))

$(foreach file, $(1), $(eval $(call ccTemplate, $(file), $(3), $(4), $(5))))

__temp_packet__ += __temp_objs__
endef

addfiles = $(eval $(call doAddFiles, $(1), $(2), $(3), $(4), $(5)))


SEMIC		:= /

# 找到@dir中的所有@types后缀的文件
# @dir = $1, @types = $2
find_list = $(filter $(if $(2),$(addprefix %.,$(2),%)), \
			$(wildcard $(addsuffix $(SEMIC)*,$(1))))

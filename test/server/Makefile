CC  := g++
LD  := g++ 
AR  := ar rucs 
RM	:= rm -rf
CFLAGS  := -Wall -g -m64
#CFLAGS := -Wall -O2 -m64
INCLUDE := -I. -I../../common/ -I../../reactor/ 
LDFLAGS := -m64 -lpthread
LIBS    := ../../run/lib/libcommon.a ../../run/lib/libreactor.a

TARGETPATH := ../../run/
$(shell mkdir -p $(TARGETPATH))
#TARGET  := $(TARGETPATH)/test
TARGET := $(TARGETPATH)/server
# 临时目标文件的存放路径
OBJPATH := ./obj
# 临时目录不存在时自动创建
$(shell mkdir -p $(OBJPATH))

AUTHOR = $(shell whoami)
VERSION = 1 #$(shell svn info | grep -v "修改的版本" | grep -v "库" | grep "版本" | grep -oP "\d+")

MARCO	:= -DK_MULTI_THREAD_ -DDAEMON -DAUTHOR=\"$(AUTHOR)\" -DSVNVERSION=$(VERSION) -DSVNLATEST=$(LATEST)
# 设置源代码所在目录，指定目录下所有子目录中的文件都将被加入项目中，若对某些目录或文件忽略需要特别指定;
# 指定目录时可使用相对路径（相对Makefile的路径），多目录可以使用空格隔开
# TODO 注意：源文件和目录名不能含有' '、'$'、'#'、'@'、'?'、'%'、'~'等特殊字符
SRCDIRS := ./
# 某些不再指定源代码目录中的文件可以额外添加
ADDITION:= 

# 可以将源代码目录中指定的子目录忽略，从而从项目中排除
EXCLUDE_DIRS:= 
# 忽略的文件，即不作为项目源文件，而又不在指定忽略目录中的文件
EXCLUDE_FILES:= 

# 自动查找所有源文件
FILES:= $(shell \
		excludes=; \
		for dir in $(EXCLUDE_DIRS); do\
			excludes=$$excludes' '`find $$dir -type f -iregex ".*\.\(c\|cc\|cpp\|cxx\)"`;\
		done;\
		for file in $(EXCLUDE_FILES); do\
			excludes=$$excludes' '$$file;\
		done;\
		for dir in $(SRCDIRS); do\
			if [ -d $$dir ]; then\
				filelist=`find $$dir -type f -iregex ".*\.\(c\|cc\|cpp\|cxx\)"`;\
				for file in $$filelist; do\
					for entry in $$excludes; do\
						if [ "$$file" -ef "$$entry" ]; then\
							continue 2;\
						fi;\
					done;\
					if [ -f $$file ]; then\
						file=`echo $$file | sed "s/^\.\///"`;\
						printf "%s " $$file;\
					fi;\
				done;\
			fi;\
		done;\
		for file in $(ADDITION); do\
			if [ -f $$file ]; then\
				file=`echo $$file | sed "s/^\.\///"`;\
				printf "%s " $$file;\
			fi;\
		done;)

# 为了能从目标文件得到源文件路径，将全路径中的'/'替换成了'~'，'.'替换成了'@'
# 因此看到有~和@符号的.o文件请不用惊讶
OBJS    := $(addprefix $(OBJPATH)/,$(addsuffix .o,$(shell echo $(FILES) | sed "s/\//\@/g" | sed "s/\./~/g")))

# 根据目标文件反向得到源文件的全路径
obj2name=$(shell echo $* | sed "s/\@/\//g" | sed "s/~/\./g")

.PHONY: list clean

all: $(OBJPATH) $(TARGET)

list:
	@echo $(FILES)

$(TARGET): $(OBJS) $(LIBS)
#	$(AR) $@ $(OBJPATH)/*.o
	$(LD) $(LDFLAGS) -o $@ $(OBJPATH)/*.o $(LIBS)

$(OBJPATH)/%.o: $(obj2name) $(OBJPATH)/%.d
	$(CC) -c -o $@ $(obj2name) $(CFLAGS) $(MARCO) $(INCLUDE)

$(OBJPATH)/%.d: $(obj2name)
	@if [ "$(MAKECMDGOALS)" != "clean" ];then\
		$(CC) $(INCLUDE) -MT $(@:.d=.o) -MF $@ -MM $(obj2name); \
		fi

-include $(OBJS:.o=.d)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJPATH)/*.o
	$(RM) $(OBJPATH)/*.d

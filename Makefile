#define build mode
#if build mode is debug,the program output moon_d,the params of compile is -c -g -Ddebug
#if build mode is release,the program output moon,the params of compile is -c -O3
#定义编译模式
#如果编译模式是debug,最终生成的程序为moon_d,编译的参数选项为-c -g -Ddebug
#如果编译模式是release，最终生成的程序为moon,编译的参数选项为-c -O3
BUILD_MODE = debug
#define compiler
#定义编译器
CC = gcc
#source code file root path
#定义源代码所在根路径
SRC_DIR = Moon
#add other include file path
#添加头文件路径
INC_DIR = \
	-I./Moon/app \
	-I./Moon/core \
	-I./Moon/module \
	-I./Moon/cfg \
	-I./Moon/collection \
	-I./Moon/msg \

#recursive 3 level sub dir
#递归查找源代码根路径下的三级子目录
DIRS := $(shell find $(SRC_DIR) -maxdepth 3 -type d)
#add ervery dir to search path
#将每个文件夹添加到搜索路径中
VPATH = $(DIRS)
#determine compile mode,gcc params
#判断编译模式,设置gcc参数
ifeq ($(BUILD_MODE),debug)
CFLAGS = -c -g -Ddebug
#define output dir
#定义程序编译输出路径
BUILD_DIR = linux_debug
TARGET = moon_d
else ifeq ($(BUILD_MODE),release)
CFLAGS = -c -O3
#define output dir
#定义程序编译输出路径
BUILD_DIR = linux_release
TARGET = moon
else
$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

#define obj file path
#定义中间目标文件路径
BUILD_OBJ = $(BUILD_DIR)/OBJ

#find all *.c files under SRC_DIR path and sub dir
#查找源代码目录下以及子目录下的所有.c文件
SOURCES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c)) 
OBJS   = $(addprefix $(BUILD_OBJ)/,$(patsubst %.c,%.o,$(notdir $(SOURCES))))

$(BUILD_DIR)/$(TARGET):$(OBJS)  
	$(CC) $^ -o $@  -lm

#if build dir not exist,and create it
#编译之前要创建OBJ目录，确保目录存在  
$(BUILD_OBJ)/%.o:%.c
	if [ ! -d $(BUILD_OBJ) ]; then mkdir -p $(BUILD_OBJ); fi;
	$(CC) -c $(CFLAGS) -o $@ $<  

.PHONY : install clean

install:
	cp -rf $(SRC_DIR)/config $(BUILD_DIR)/
	cp -rf $(SRC_DIR)/image $(BUILD_DIR)/
	mkdir $(BUILD_DIR)/log

clean:
	rm -rf $(BUILD_DIR)
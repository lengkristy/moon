/****************************************************************************
作者：代浩然
时间：2017-8-3
该文件定义为栈的相关实现
栈的特性：
	1、先进后出，后进先出
****************************************************************************/
#pragma once
#ifndef _STACK_H
#define _STACK_H
#ifdef __cplusplus
extern "C" {
#endif

//定义栈的初始化大小
#define STACK_INIT_SIZE 100
//栈空间慢只有，每次递增的大小
#define STACK_INCREASE_SIZE 200

/**
 * 定义栈节点
 */
typedef struct _Stack_Node{
	void* data;//数据域
}Stack_Node;

/**
 * 定义栈结构
 */
typedef struct _Stack{
	Stack_Node* node;//栈的数据域
	unsigned long base;//栈底指针
	unsigned long top;//栈顶指针
	unsigned long size;//栈大小
}Stack;

/**
 * 功能：初始化栈
 * 返回值：如果成功，则返回栈的地址，如果失败返回NULL
 */
Stack* Stack_Init();

/**
 * 功能：将数据压栈
 * 参数：
 *		stack：栈指针
 *		data：被压入栈的数据地址
 */
void Stack_Push(Stack* stack,void* data);

/**
 * 功能：将数据弹出栈
 * 参数：
 *		stack：栈指针
 * 返回值：返回的数据的地址，如果栈没有数据则返回NULL
 */
void* Stack_Pop(Stack* stack);

/**
 * 功能：清空栈
 * 参数：
 *		stack：栈指针
 */
void Stack_Clear(Stack* stack);

/**
 * 功能：释放栈
 * 参数：
 *		stack：栈指针
 */
void Stack_Free(Stack* stack);

/**
 * 功能：栈测试
 */
void Stack_Test();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
#endif
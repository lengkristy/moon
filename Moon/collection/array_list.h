/****************************************************************************
作者：代浩然
时间：2017-8-2
该文件定义为线性链表相关的实现
线性链表的特性：
	1、在内存中的存放地址是连续的
优点：
	1、由于地址的连续性，所以CPU的pc指针寻址的地址空间的范围不会太大，所以随机访问链表的速度非常快，遍历的速度非常快，释放空间也比较快
缺点：
	2、也正是地址的连续性，所以当每次随机插入的时候都要进行链表剩余空间的检查，如果空间不够，然后又要重新分配内存，
	   如果随机插入的位置后面有元素，需要把该位置的元素都要依次往后挪一位，然后在当前位置进行插入，所以这样就造成
	   速度非常慢
****************************************************************************/
#pragma once
#ifndef _ARRAY_LIST_H
#define _ARRAY_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

//链表初始化大小
#define ARRAY_LIST_INIT_SIZE 100
//链表存满每次增加的大小
#define ARRAY_LIST_INCREASE_SIZE 200

//定义节点
typedef struct _Array_Node{
	void* data;//数据域
}Array_Node;

//定义链表结构
typedef struct _Array_List{
	Array_Node *node;//数据域
	unsigned long length;//链表的当前长度
	unsigned long size;//链表的整体大小
}Array_List;

/**
 * 功能：初始化链表
 * 返回值：如果成功，则返回链表的地址，如果失败返回NULL
 */
Array_List* array_list_init();

/**
 * 功能：随机插入链表
 * 参数：
 *		pList：链表地址
 *		pData：插入的数据节点
 *		index：要插入的位置，如果为0，则默认从链表的开始处插入，如果为-1，则默认从链表的最后插入
 * 返回值：成功返回0，失败返回-1
 */
int array_list_insert(Array_List* pList,void* pData,long index);

/**
 * 功能：从某个位置取出元素
 * 参数：
 *		pList：链表地址
 *		index：位置
 * 返回值：返回数据体指针
 */
void* array_list_getAt(Array_List* pList,unsigned long index);

/**
 * 功能：通过索引删除元素，删除元素只是将data域置为NULL，并不会释放data指针，由调用者释放
 * 参数：
 *		pList：链表地址
 *		index：位置
 */
void array_list_removeAt(Array_List* pList,unsigned long index);

/**
 * 功能：移除某个元素
 * 参数：
 *     pList：链表地址
 *	   pData：元素指针
 */
void array_list_remove(Array_List* pList,void* pData);

/**
 * 功能：清空链表
 * 参数：
 *	pList：链表地址
 */
void array_list_clear(Array_List* pList);

/**
 * 功能：释放链表空间
 * 参数：
 *	pList：链表地址
 */
void array_list_free(Array_List* pList);

/**
 * 功能：Array测试
 */
void array_list_test();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif

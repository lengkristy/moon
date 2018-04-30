/****************************************************************************
作者：代浩然
时间：2017-8-2
该文件定义为非线性链表相关的实现
线性链表的特性：
	1、在内存中的存放地址是非连续的，随机分配
优点：
	1、由于地址的非连续性，所以我们在随机插入的时候只需要找到插入的位置，然后将该位置的前后指针的位置只想该元素的地址就行，不需要重新分配内存
	   ，一瞬间就能完成，速度非常快
缺点：
	2、由于地址的非连续性，所以元素在内存中的存放位置是非固定的，所以就造成了第一个元素和第二个元素的内存位置跨度非常大，最大为4GB，所以CPU的PC
	   指针寻址非常慢。造成随机访问慢、遍历效率低下，释放空间效率低下
****************************************************************************/
#pragma once
#ifndef _Link_LIST_H
#define _Link_LIST_H
#ifdef __cplusplus
extern "C" {
#endif

//定义节点，双向链表
typedef struct _Link_Node{
	void* data;//数据域
	struct _Link_Node* priorNode;//指向上一个节点的指针
	struct _Link_Node* nextNode;//指向下一个节点的指针
}Link_Node;

//定义链表结构
typedef struct _Link_List{
	Link_Node *head;//链表的头指针
	Link_Node *trail;//链表的尾部指针
	unsigned long length;//链表的当前长度
}Link_List;

/**
 * 功能：初始化链表
 * 返回值：如果成功，则返回链表的地址，如果失败返回NULL
 */
Link_List* link_list_init();

/**
 * 功能：随机插入链表
 * 参数：
 *		pList：链表地址
 *		pData：插入的数据节点
 *		index：要插入的位置，如果为0，则默认从链表的开始处插入，如果为-1，则默认从链表的最后插入
 * 返回值：成功返回0，失败返回-1
 */
int link_list_insert(Link_List* pList,void* pData,long index);

/**
 * 功能：从某个位置取出元素
 * 参数：
 *		pList：链表地址
 *		index：位置
 * 返回值：返回数据体指针
 */
void* link_list_getAt(Link_List* pList,unsigned long index);

/**
 * 功能：通过索引删除元素，删除元素只是将data域置为NULL，并不会释放data指针，由调用者释放
 * 参数：
 *		pList：链表地址
 *		index：位置
 */
void link_list_removeAt(Link_List* pList,unsigned long index);


/**
 * 功能：清空链表
 * 参数：
 *	pList：链表地址
 */
void link_list_clear(Link_List* pList);

/**
 * 功能：释放链表空间
 * 参数：
 *	pList：链表地址
 */
void link_list_free(Link_List* pList);

/**
 * 功能：Link_List测试
 */
void link_list_test();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
#endif
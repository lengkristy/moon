/*********************************************************************************
 *  作者：代浩然
 *	时间：2017-08-05
 *	说明：该文件为队列的实现，实现方式是链表的双向队列，可以从链表的两端存，也可以从链表的两端取
 *
 *
 *********************************************************************************/
#pragma once
#ifndef _QUEUE_H
#define _QUEUE_H
#ifdef __cplusplus
extern "C" {
#endif

//定义bool类型
#ifndef bool
#define bool int
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
	

	/**
	 *  说明：定义队列添加的回调通知的函数指针类型
	 *  参数：
	 *		queue：新增队列的队列指针
	 *		data：被新增的数据
	 *	返回值：处理成功则返回0，失败则返回-1
	 */
	typedef int(*QueueIncreased)(void* queue,void* data);

	//队列节点
	typedef struct _QueueNode{
		void* data; //数据域
		struct _QueueNode* next;//指向下一个节点
		struct _QueueNode* prior;//指向上一个节点
	}QueueNode;

	//队列的结构体
	typedef struct _Queue{
		QueueNode* head;//队列的头部
		QueueNode* tail;//队列的尾部
		unsigned long length;//当前队列的长度
		QueueIncreased onQueueIncreased;//函数指针
	}Queue;

	

	/**
	 *  说明：初始化队列
	 *	参数：
	 *		queueIncreasedEvent：需要注册的队列新增事件
	 *	返回值：成功则返回队列的指针，失败则返回NULL
	 */
	Queue* Queue_Init(QueueIncreased queueIncreasedEvent);

	/**
	 *  说明：从队列的头部添加
	 *	参数：
	 *		queue：队列指针
	 *		data：添加的数据域
	 *	返回值：添加成功返回0，失败返回-1
	 */
	int Queue_AddToHead(Queue* queue,void* data);

	/**
	 *  说明：从队列的尾部添加
	 *	参数：
	 *		queue：队列指针
	 *		data：添加的数据域
	 *	返回值：添加成功返回0，失败返回-1
	 */
	int Queue_AddToTail(Queue* queue,void* data);

	/**
	 *  说明：从队列的头部取出数据
	 *	参数：
	 *		queue：队列指针
	 *	返回值：成功取出数据，失败返回NULL
	 */
	void* Queue_GetFromHead(Queue* queue);

	/**
	 *  说明：从队列的尾部取出数据
	 *	参数：
	 *		queue：队列指针
	 *	返回值：成功取出数据，失败返回NULL
	 */
	void* Queue_GetFromTail(Queue* queue);

	/**
	 *  说明：队列释放
	 *	参数：
	 *		queue：队列指针
	 *		isFreeData:是否自动释放data域
	 */
	void Queue_Free(Queue* queue,bool isFreeData);

	//队列新增事件
	int OnQueueIncreasedEvent(void* queue,void* data);

	//测试
	void Queue_Test();

#ifdef __cplusplus
}
#endif
#endif
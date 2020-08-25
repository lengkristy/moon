#include "queue.h"
#include <stdlib.h>
#include <malloc.h>

#include "../module/moon_string.h"

#ifdef MS_WINDOWS/* only support win32 and greater. */
#include <windows.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

/**
	*  说明：初始化队列
	*	参数：
	*		queueIncreasedEvent：需要注册的队列新增事件
	*	返回值：成功则返回队列的指针，失败则返回NULL
	*/
Queue* Queue_Init(QueueIncreased queueIncreasedEvent)
{
	Queue* queue = NULL;
	moon_char muuid[50] = {0};
#ifdef MS_WINDOWS
	wchar_t event_name[100] = {0};
#endif
	queue = (Queue*)malloc(sizeof(Queue));
	if(queue == NULL)
	{
		return NULL;
	}
	queue->length = 0;
	queue->head = NULL;
	queue->tail = NULL;
	queue->onQueueIncreased = queueIncreasedEvent;

#ifdef MS_WINDOWS
	moon_create_32uuid(muuid);
	moon_ms_windows_utf8_to_unicode(muuid,event_name);
	queue->hQueueEvent = CreateEvent(NULL, FALSE, TRUE,event_name);
	if (queue->hQueueEvent == NULL)
	{
		Queue_Free(queue,true);
		return NULL;
	}
#endif
	return queue;
}
 
/**
	*  说明：从队列的头部添加
	*	参数：
	*		queue：队列指针
	*		data：添加的数据域
	*	返回值：添加成功返回0，失败返回-1
	*/
int Queue_AddToHead(Queue* queue,void* data)
{
	QueueNode* node = NULL;
	if(queue == NULL)
		return -1;
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	//创建节点
	node = (QueueNode*)malloc(sizeof(QueueNode));
	
	if(node == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return -1;
	}
	node->data = data;
	//如果是第一次添加，头部和尾部都指向当前节点
	if(queue->tail == NULL && queue->head == NULL)
	{
		queue->tail = node;
		queue->head = node;
		node->next = NULL;
		node->prior = NULL;
	}
	else
	{
		//开始从头部添加
		queue->head->prior = node;
		node->next = queue->head;
		node->prior = NULL;
		queue->head = node;
	}
	queue->length++;
	//回调添加事件
	if(queue->onQueueIncreased != NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return queue->onQueueIncreased(queue,data);
	}
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
	return 0;
}
 
/**
	*  说明：从队列的尾部添加
	*	参数：
	*		queue：队列指针
	*		data：添加的数据域
	*	返回值：添加成功返回0，失败返回-1
	*/
int Queue_AddToTail(Queue* queue,void* data)
{
	QueueNode* node = NULL;
	if(queue == NULL)
		return -1;
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	//创建节点
	node = (QueueNode*)malloc(sizeof(QueueNode));
	if(node == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return -1;
	}
	node->data = data;
	//如果是第一次添加，头部和尾部都指向当前节点
	if(queue->tail == NULL && queue->head == NULL)
	{
		queue->tail = node;
		queue->head = node;
		node->next = NULL;
		node->prior = NULL;
	}
	else
	{
		//开始从尾部添加
		queue->tail->next = node;
		node->prior = queue->tail;
		node->next = NULL;
		queue->tail = node;
	}
	queue->length++;
	//回调添加事件
	if(queue->onQueueIncreased != NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return queue->onQueueIncreased(queue,data);
	}
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
	return 0;
}
 
/**
	*  说明：从队列的头部取出数据
	*	参数：
	*		queue：队列指针
	*	返回值：成功取出数据，失败返回NULL
	*/
void* Queue_GetFromHead(Queue* queue)
{
	void* data = NULL;
	QueueNode* node = NULL; 
	if(queue == NULL || queue->head == NULL)
	{
		return NULL;
	}
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	node = queue->head;
	queue->head = node->next;
	if(queue->head != NULL)//如果取出的头部不为空，将头部的上一个节点置为NULL
	{
		queue->head->prior = NULL;
	}
	else
	{
		//表示队列已经取完了
		queue->tail = NULL;
		queue->head = NULL;
	}
	data = node->data;
	free(node);
	queue->length--;
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
	return data;
}
 
 
/**
	*  说明：从队列的尾部取出数据
	*	参数：
	*		queue：队列指针
	*	返回值：成功取出数据，失败返回NULL
	*/
void* Queue_GetFromTail(Queue* queue)
{
	void* data = NULL;
	QueueNode* node = NULL; 
	if(queue == NULL || queue->tail == NULL)
	{
		return NULL;
	}
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	node = queue->tail;
	if(node != NULL)
	{
		queue->tail = node->prior;
		if(queue->tail != NULL)//如果取出的尾部不为空，将尾部的下一个节点置为NULL
		{
			queue->tail->next = NULL;
		}
		else
		{
			//表示队列已经取完了
			queue->tail = NULL;
			queue->head = NULL;
		}
		data = node->data;
		free(node);
		queue->length--;
	}
#ifdef MS_WINDOWS
	SetEvent(queue->hQueueEvent);
#endif
	return data;
}
 
/**
	*  说明：队列释放
	*	参数：
	*		queue：队列指针
	*		isFreeData:是否自动释放data域
	*/
void Queue_Free(Queue* queue,bool isFreeData)
{
	void* data = NULL;
	if(queue == NULL)
		return;
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	data = Queue_GetFromHead(queue);
	while(data != NULL && isFreeData)
	{
		free(data);
		data = Queue_GetFromHead(queue);
	}
 #ifdef MS_WINDOWS
	SetEvent(queue->hQueueEvent);
#endif
	free(queue);
}
 
//队列新增事件
int OnQueueIncreasedEvent(void* queue,void* data)
{
	Queue* q = (Queue*)queue;
	int *p = (int*)data;
	return 0;
}
 
//测试
void Queue_Test()
{
	void* data = NULL;
	Queue* queue = NULL;
	int i = 0,j = 1,k = 2,l = 3;
	int* p = NULL;
	queue = Queue_Init(OnQueueIncreasedEvent);
	Queue_AddToHead(queue,&i);
	Queue_AddToHead(queue,&j);
	Queue_AddToTail(queue,&k);
	Queue_AddToTail(queue,&l);
	data = Queue_GetFromHead(queue);
	while(data != NULL)
	{
		p = (int *)data;
		//printf("%d\t",*p);
		data = Queue_GetFromHead(queue);//从头取
	}
	//printf("\n");
	Queue_Free(queue,true);
	queue = Queue_Init(NULL);
	Queue_AddToHead(queue,&i);
	Queue_AddToHead(queue,&j);
	Queue_AddToTail(queue,&k);
	Queue_AddToTail(queue,&l);
	data = Queue_GetFromHead(queue);
	while(data != NULL)
	{
		p = (int *)data;
		//printf("%d\t",*p);
		data = Queue_GetFromTail(queue);//从头取
	}
	//printf("\n");
 
	Queue_AddToTail(queue,&i);
	Queue_AddToTail(queue,&j);
	Queue_AddToTail(queue,&k);
	Queue_AddToTail(queue,&l);
	data = Queue_GetFromHead(queue);
	while(data != NULL)
	{
		p = (int *)data;
		//printf("%d\t",*p);
		data = Queue_GetFromHead(queue);//从头取
	}
	//printf("\n");
	Queue_Free(queue,true);
	//getchar();
}

#ifdef __cplusplus
}
#endif
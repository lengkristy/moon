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
	*  ˵������ʼ������
	*	������
	*		queueIncreasedEvent����Ҫע��Ķ��������¼�
	*	����ֵ���ɹ��򷵻ض��е�ָ�룬ʧ���򷵻�NULL
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
	*  ˵�����Ӷ��е�ͷ�����
	*	������
	*		queue������ָ��
	*		data����ӵ�������
	*	����ֵ����ӳɹ�����0��ʧ�ܷ���-1
	*/
int Queue_AddToHead(Queue* queue,void* data)
{
	QueueNode* node = NULL;
	if(queue == NULL)
		return -1;
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	//�����ڵ�
	node = (QueueNode*)malloc(sizeof(QueueNode));
	
	if(node == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return -1;
	}
	node->data = data;
	//����ǵ�һ����ӣ�ͷ����β����ָ��ǰ�ڵ�
	if(queue->tail == NULL && queue->head == NULL)
	{
		queue->tail = node;
		queue->head = node;
		node->next = NULL;
		node->prior = NULL;
	}
	else
	{
		//��ʼ��ͷ�����
		queue->head->prior = node;
		node->next = queue->head;
		node->prior = NULL;
		queue->head = node;
	}
	queue->length++;
	//�ص�����¼�
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
	*  ˵�����Ӷ��е�β�����
	*	������
	*		queue������ָ��
	*		data����ӵ�������
	*	����ֵ����ӳɹ�����0��ʧ�ܷ���-1
	*/
int Queue_AddToTail(Queue* queue,void* data)
{
	QueueNode* node = NULL;
	if(queue == NULL)
		return -1;
#ifdef MS_WINDOWS
	WaitForSingleObject(queue->hQueueEvent, INFINITE);
#endif
	//�����ڵ�
	node = (QueueNode*)malloc(sizeof(QueueNode));
	if(node == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(queue->hQueueEvent);
#endif
		return -1;
	}
	node->data = data;
	//����ǵ�һ����ӣ�ͷ����β����ָ��ǰ�ڵ�
	if(queue->tail == NULL && queue->head == NULL)
	{
		queue->tail = node;
		queue->head = node;
		node->next = NULL;
		node->prior = NULL;
	}
	else
	{
		//��ʼ��β�����
		queue->tail->next = node;
		node->prior = queue->tail;
		node->next = NULL;
		queue->tail = node;
	}
	queue->length++;
	//�ص�����¼�
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
	*  ˵�����Ӷ��е�ͷ��ȡ������
	*	������
	*		queue������ָ��
	*	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL
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
	if(queue->head != NULL)//���ȡ����ͷ����Ϊ�գ���ͷ������һ���ڵ���ΪNULL
	{
		queue->head->prior = NULL;
	}
	else
	{
		//��ʾ�����Ѿ�ȡ����
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
	*  ˵�����Ӷ��е�β��ȡ������
	*	������
	*		queue������ָ��
	*	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL
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
		if(queue->tail != NULL)//���ȡ����β����Ϊ�գ���β������һ���ڵ���ΪNULL
		{
			queue->tail->next = NULL;
		}
		else
		{
			//��ʾ�����Ѿ�ȡ����
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
	*  ˵���������ͷ�
	*	������
	*		queue������ָ��
	*		isFreeData:�Ƿ��Զ��ͷ�data��
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
 
//���������¼�
int OnQueueIncreasedEvent(void* queue,void* data)
{
	Queue* q = (Queue*)queue;
	int *p = (int*)data;
	return 0;
}
 
//����
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
		data = Queue_GetFromHead(queue);//��ͷȡ
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
		data = Queue_GetFromTail(queue);//��ͷȡ
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
		data = Queue_GetFromHead(queue);//��ͷȡ
	}
	//printf("\n");
	Queue_Free(queue,true);
	//getchar();
}

#ifdef __cplusplus
}
#endif
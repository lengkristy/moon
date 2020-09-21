/********************************************************************************
 * ģ��˵����
 *		��ģ��Ϊ���е�ʵ��
 *
 ********************************************************************************/
#ifndef _QUEUE_H
#define _QUEUE_H

#ifdef _MSC_VER/* only support win32 and greater. */
#define MS_WINDOWS
#endif

#ifdef __cplusplus
extern "C" {
#endif


//����bool����
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
	 *  ˵�������������ӵĻص�֪ͨ�ĺ���ָ������
	 *  ������
	 *		queue���������еĶ���ָ��
	 *		data��������������
	 *	����ֵ������ɹ��򷵻�0��ʧ���򷵻�-1
	 */
	typedef int(*QueueIncreased)(void* queue,void* data);
 
	//���нڵ�
	typedef struct _QueueNode{
		void* data; //������
		struct _QueueNode* next;//ָ����һ���ڵ�
		struct _QueueNode* prior;//ָ����һ���ڵ�
	}QueueNode;
 
	//���еĽṹ��
	typedef struct _Queue{
		QueueNode* head;//���е�ͷ��
		QueueNode* tail;//���е�β��
		unsigned long length;//��ǰ���еĳ���
		QueueIncreased onQueueIncreased;//����ָ��
		void* hQueueEvent;//ͬ��
	}Queue;
 
	
 
	/**
	 *  ˵������ʼ������
	 *	������
	 *		queueIncreasedEvent����Ҫע��Ķ��������¼�
	 *	����ֵ���ɹ��򷵻ض��е�ָ�룬ʧ���򷵻�NULL
	 */
	Queue* Queue_Init(QueueIncreased queueIncreasedEvent);
 
	/**
	 *  ˵�����Ӷ��е�ͷ�����
	 *	������
	 *		queue������ָ��
	 *		data����ӵ�������
	 *	����ֵ����ӳɹ�����0��ʧ�ܷ���-1
	 */
	int Queue_AddToHead(Queue* queue,void* data);
 
	/**
	 *  ˵�����Ӷ��е�β�����
	 *	������
	 *		queue������ָ��
	 *		data����ӵ�������
	 *	����ֵ����ӳɹ�����0��ʧ�ܷ���-1
	 */
	int Queue_AddToTail(Queue* queue,void* data);
 
	/**
	 *  ˵�����Ӷ��е�ͷ��ȡ������
	 *	������
	 *		queue������ָ��
	 *	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL
	 */
	void* Queue_GetFromHead(Queue* queue);
 
	/**
	 *  ˵�����Ӷ��е�β��ȡ������
	 *	������
	 *		queue������ָ��
	 *	����ֵ���ɹ�ȡ�����ݣ�ʧ�ܷ���NULL
	 */
	void* Queue_GetFromTail(Queue* queue);
 
	/**
	 *  ˵���������ͷ�
	 *	������
	 *		queue������ָ��
	 *		isFreeData:�Ƿ��Զ��ͷ�data��
	 */
	void Queue_Free(Queue* queue,bool isFreeData);
 
	//���������¼�
	int OnQueueIncreasedEvent(void* queue,void* data);
 
	//����
	void Queue_Test();


#ifdef __cplusplus
}
#endif

#endif
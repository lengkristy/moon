#include "link_list.h"
#include <stdlib.h>
#include <malloc.h>

#ifdef _MSC_VER/* only support win32 and greater. */
#include <windows.h>
#define MS_WINDOWS
#define LINK_LIST_MUTEX "LINK_LIST_MUTEX"
static HANDLE g_hMutex;
#endif

/**
 * 功能：初始化链表
 * 返回值：如果成功，则返回链表的地址，如果失败返回NULL
 */
Link_List* link_list_init()
{
	Link_List* pList = NULL;
	pList = (Link_List*)malloc(sizeof(Link_List));
	if(pList == NULL)
	{
		return NULL;
	}
	pList->length = 0;
	pList->head = NULL;
	pList->trail = NULL;
	//初始化互斥体
#ifdef MS_WINDOWS
	g_hMutex = CreateMutex(NULL, FALSE,  TEXT(LINK_LIST_MUTEX));
	if (g_hMutex == NULL)
	{
		link_list_free(pList);
		return NULL;
	}
#endif
	return pList;
}

/**
 * 功能：随机插入链表
 * 参数：
 *		pList：链表地址
 *		pData：插入的数据节点
 *		index：要插入的位置，如果为0，则默认从链表的开始处插入，如果为-1，则默认从链表的最后插入
 * 返回值：成功返回0，失败返回-1
 */
int link_list_insert(Link_List* pList,void* pData,long index)
{
	long i = 0;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LINK_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return -1;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	if(pList == NULL)
		return -1;
	if(index < -1 || (index > pList->length && index != -1))
	{
		return -1;
	}
	//判断是否是首次插入
	if(pList->length == 0)
	{
		Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
		if(pNode == NULL)
			return -1;
		pNode->data = pData;
		pNode->priorNode = NULL;
		pNode->nextNode = NULL;
		pList->head = pNode;
		pList->trail = pNode;
		pList->length++;
		return 0;
	}
	else
	{
		if(-1 == index)//从末尾处插入
		{
			//创建节点
			Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
			if(pNode == NULL)
				return -1;
			pNode->data = pData;
			pNode->nextNode = NULL;
			pNode->priorNode = pList->trail;
			//将节点加到末尾处
			pList->trail->nextNode = pNode;
			pList->trail = pNode;
			pList->length++;
		}
		else if(0 == index) //从开始处插入
		{
			//创建节点
			Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
			if(pNode == NULL)
				return -1;
			pNode->data = pData;
			pNode->nextNode = pList->head;
			pNode->priorNode = NULL;
			//将节点加载到头部
			pList->head->priorNode = pNode;
			pList->head = pNode;
			pList->length++;
			return 0;
		}
		else//指定位置插入
		{
			//后期可以使用快速查找算法优化
			Link_Node* pNode = pList->head;
			i=0;
			while(pNode != NULL)
			{
				if(index == i)//查找到要插入的位置节点
				{
					//创建节点
					Link_Node* pCurrentNode = (Link_Node*)malloc(sizeof(Link_Node));
					if(pCurrentNode == NULL)
						return -1;
					pCurrentNode->nextNode = pNode;
					pCurrentNode->priorNode = pNode->priorNode;
					pNode->priorNode->nextNode = pCurrentNode;
					pNode->priorNode = pCurrentNode;
					pList->length++;
					return 0;
				}
				pNode = pNode->nextNode;
				i++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
	return 0;
}

/**
 * 功能：从某个位置取出元素
 * 参数：
 *		pList：链表地址
 *		index：位置
 * 返回值：返回数据体指针
 */
void* link_list_getAt(Link_List* pList,unsigned long index)
{
	int i = 0;
	Link_Node* pNode = NULL;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LINK_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return NULL;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	if(pList == NULL)
	{
		return NULL;
	}
	i = 0;
	pNode = pList->head;
	while(pNode != NULL)
	{
		if(i == index)
		{
			return pNode->data;
		}
		pNode = pNode->nextNode;
		i++;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
	return NULL;
}

/**
 * 功能：通过索引删除元素，删除元素只是将data域置为NULL，并不会释放data指针，由调用者释放
 * 参数：
 *		pList：链表地址
 *		index：位置
 */
void link_list_removeAt(Link_List* pList,unsigned long index)
{
	int i = 0;
	Link_Node* pNode = NULL;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LINK_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return NULL;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	if(pList == NULL)
	{
		return;
	}
	i = 0;
	pNode = pList->head;
	while(pNode != NULL)
	{
		if(i == index)
		{
			//找到节点，开始删除
			if(pNode->priorNode == NULL) //表示链表头部节点
			{
				Link_Node* head = pList->head;
				pList->head = head->nextNode;
				pList->head->priorNode = NULL;
				free(head);//释放节点
				pList->length--;
			}
			else if(pNode->nextNode == NULL)//表示链表尾部节点
			{
				Link_Node* trial = pList->trail;
				pList->trail = trial->priorNode;
				pList->trail->nextNode = NULL;
				free(trial);//释放节点
				pList->length--;
			}
			else
			{
				//当前节点的上一个节点的下一个节点指向当前节点的下一个节点
				pNode->priorNode->nextNode = pNode->nextNode;
				//当前节点的下一个节点的上一个节点指向当前节点的上一个节点
				pNode->nextNode->priorNode = pNode->priorNode;
				free(pNode);
				pList->length--;
			}
			return;
		}
		pNode = pNode->nextNode;
		i++;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/**
 * 功能：清空链表
 * 参数：
 *	pList：链表地址
 */
void link_list_clear(Link_List* pList)
{
	//将数据域置为空
	Link_Node* pNode = NULL;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(LINK_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return NULL;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	if(pList == NULL)
	{
		return;
	}
	//从尾部开始释放
	pNode = pList->trail;
	while(pNode != NULL)
	{
		pList->trail = pNode->priorNode;
		free(pNode);
		pNode = pList->trail;
	}
	pList->length = 0;
	pList->head = NULL;
	pList->trail = NULL;

	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/**
 * 功能：释放链表空间
 * 参数：
 *	pList：链表地址
 */
void link_list_free(Link_List* pList)
{
	Link_Node* pNode = NULL;
	if(pList == NULL)
	{
		return;
	}
	//从尾部开始释放
	pNode = pList->trail;
	while(pNode != NULL)
	{
		pList->trail = pNode->priorNode;
		free(pNode);
		pNode = pList->trail;
	}
	pList->length = 0;
	pList->head = NULL;
	pList->trail = NULL;
	free(pList);
	pList = NULL;
#ifdef MS_WINDOWS
	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}
#endif
}


/**
 * 功能：Link_List测试
 */
void link_list_test()
{
	int i = 0;
	int *p = NULL;
	Link_List* list = link_list_init();
	//动态添加1000个值
	for(i = 0;i < 1000;i++)
	{
		p = (int*) malloc(sizeof(int));
		*p = i;
		link_list_insert(list,p,-1);
	}
	//取出第500个元素
	p = (int*)link_list_getAt(list,499);
	//删除第500个元素，删除之后记得释放
	link_list_removeAt(list,499);
	free(p);
	//重新获取第500个元素
	p = (int*)link_list_getAt(list,499);
	//开始释放空间
	for(i = 0;i<list->length;i++)
	{
		p = (int*)link_list_getAt(list,i);
		free(p);
	}
	link_list_clear(list);
	link_list_free(list);
}
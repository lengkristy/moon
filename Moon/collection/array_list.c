#include "array_List.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>


#ifdef _MSC_VER/* only support win32 and greater. */
#include <windows.h>
#define MS_WINDOWS
#define ARRAY_LIST_MUTEX "ARRAY_LIST_MUTEX"
static HANDLE g_hMutex;
#endif

/**
 * 功能：初始化链表
 * 返回值：如果成功，则返回链表的地址，如果失败返回NULL
 */
Array_List* array_list_init()
{
	int i = 0;
	Array_List* pList = (Array_List*)malloc(sizeof(Array_List));//配置链表空间
	if(pList == NULL)
	{
		return NULL;
	}
	//分配Node节点
	pList->node = (Array_Node*)malloc(ARRAY_LIST_INIT_SIZE * sizeof(Array_Node));//分配存放数据域的空间
	if(pList->node == NULL)
	{
		free(pList);
		return NULL;
	}
	//将Node的data域置为空置
	for(i = 0;i < ARRAY_LIST_INIT_SIZE;i++)
	{
		pList->node[i].data = NULL;
	}
	pList->length = 0;
	pList->size = ARRAY_LIST_INIT_SIZE;

	//初始化互斥体
#ifdef MS_WINDOWS
	g_hMutex = CreateMutex(NULL, FALSE,  TEXT(ARRAY_LIST_MUTEX));
	if (g_hMutex == NULL)
	{
		array_list_free(pList);
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
int array_list_insert(Array_List* pList,void* pData,long index)
{
	long i = 0;
	unsigned long reallocSize = 0;//重新分配空间的大小
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(ARRAY_LIST_MUTEX));
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
	//判断链表空间够不够长
	if(pList->length == pList->size - 1)
	{
		//重新分配空间
		reallocSize = pList->size + ARRAY_LIST_INCREASE_SIZE;
		pList->node = (Array_Node*)realloc(pList->node,reallocSize * sizeof(Array_Node));
		if(pList->node == NULL)
		{
			return -1;
		}
		//并且将新增节点的data域置为空
		for(i = pList->length;i < reallocSize;i++)
		{
			pList->node[i].data = NULL;
		}
		pList->size = reallocSize;
	}
	//开始插入
	if(index == -1)//从末尾处插入
	{
		pList->node[pList->length].data = pData;
		pList->length++;//长度自增1
		return 0;
	}
	else if(index == 0) //从开始处插入
	{
		for(i = pList->length;i >0;i--)
		{
			pList->node[i] = pList->node[i - 1];
		}
		pList->node[0].data = pData;
		pList->length++;//长度自增1
		return 0;
	}
	else//指定位置插入
	{
		for(i = pList->length;i > index;i--)
		{
			pList->node[i] = pList->node[i - 1];
		}
		pList->node[i].data = pData;
		pList->length++;//长度自增1
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
	return 0;
}

/**
 * 功能：通过索引删除元素，删除元素只是将data域置为NULL，并不会释放data指针，由调用者释放
 * 参数：
 *		pList：链表地址
 *		index：位置
 */
void array_list_removeAt(Array_List* pList,unsigned long index)
{
	int i = 0;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(ARRAY_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	if(pList == NULL)
		return;
	if(index < 0 || index >= pList->length)
		return;
	for(i = index; i < pList->length;i++)//让删除的索引后的元素依次往前移
	{
		pList->node[i] = pList->node[i + 1];
	}
	//将最后一位置空
	pList->node[pList->length - 1].data = NULL;
	//长度减1
	pList->length--;
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/**
 * 功能：移除某个元素
 * 参数：
 *     pList：链表地址
 *	   pData：元素指针
 */
void array_list_remove(Array_List* pList,void* pData)
{
	int i = 0;
	int removeIndex = -1;//需要被删除的元素索引
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(ARRAY_LIST_MUTEX));
	if(hMutex == NULL)
	{
		return;
	}
	//WaitforsingleObject将等待指定的一个mutex，直至获取到拥有权
	//通过互斥锁保证除非输出工作全部完成，否则其他线程无法输出。
	WaitForSingleObject(hMutex, 1000);
#endif
	//////////////////////////////////////////////////////////////////////////
	//临界区
	for (i = 0;i < pList->length;i++)
	{
		if(array_list_getAt(pList,i) == pData)
		{
			removeIndex = i;
			break;
		}
	}
	if(removeIndex != -1)
	{
		if(pList == NULL)
			return;
		for(i = removeIndex; i < pList->length;i++)//让删除的索引后的元素依次往前移
		{
			pList->node[i] = pList->node[i + 1];
		}
		//将最后一位置空
		pList->node[pList->length - 1].data = NULL;
		//长度减1
		pList->length--;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/**
 * 功能：从某个位置取出元素
 * 参数：
 *		pList：链表地址
 *		index：位置
 */
void* array_list_getAt(Array_List* pList,unsigned long index)
{
	void* pData = NULL;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(ARRAY_LIST_MUTEX));
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
	if(index < 0 || index >= pList->length)
	{
		return NULL;
	}
	pData = pList->node[index].data;
	
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
	return pData;
}

/**
 * 功能：清空链表
 * 参数：
 *	pList：链表地址
 */
void array_list_clear(Array_List* pList)
{
	int i = 0;
	//需要做线程同步
#ifdef MS_WINDOWS
	HANDLE hMutex = OpenMutex(SYNCHRONIZE , TRUE, TEXT(ARRAY_LIST_MUTEX));
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
	//将数据域置为空
	for(i = 0;i < pList->length;i++)
	{
		if(pList->node[i].data != NULL)
		{
			pList->node[i].data = NULL;
		}
	}
	pList->length = 0;

	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	ReleaseMutex(hMutex);
	CloseHandle(hMutex);
#endif
}

/**
 * 功能：释放链表空间，由创建的线程释放
 * 参数：
 *	pList：链表地址
 */
void array_list_free(Array_List* pList)
{
	if(pList == NULL)
		return;
	//释放节点
	if(pList->node != NULL)
	{
		free(pList->node);
		pList->node = NULL;
	}
	if(pList != NULL)
	{
		free(pList);
		pList = NULL;
	}
	if (g_hMutex != NULL)
	{
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}
}

/**
 * 功能：Array_List测试
 */
void array_list_test()
{
	int i = 0;
	int *p = NULL;
	Array_List* list = array_list_init();
	//动态添加1000个值
	for(i = 0;i < 1000;i++)
	{
		p = (int*) malloc(sizeof(int));
		*p = i;
		array_list_insert(list,p,-1);
	}
	//取出第500个元素
	p = (int*)array_list_getAt(list,499);
	//删除第500个元素，删除之后记得释放
	array_list_removeAt(list,499);
	free(p);
	//重新获取第500个元素
	p = (int*)array_list_getAt(list,499);
	//开始释放空间
	for(i = 0;i<list->length;i++)
	{
		p = (int*)array_list_getAt(list,i);
		free(p);
	}
	array_list_clear(list);
	array_list_free(list);
}
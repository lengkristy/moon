#include "array_list.h"
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "../module/moon_string.h"

#ifdef _MSC_VER/* only support win32 and greater. */
#include <windows.h>
#define MS_WINDOWS
static HANDLE g_hArrayListEvent;
#endif

/**
 * function desc:
 * 		init list
 * return:
 * 		if success return the list address，and return NULL if failed
 */
Array_List* array_list_init()
{
	int i = 0;
	moon_char muuid[50] = {0};
	Array_List* pList = (Array_List*)malloc(sizeof(Array_List));//Allocate the list address space
	if(pList == NULL)
	{
		return NULL;
	}
	//create node
	pList->node = (Array_Node*)malloc(ARRAY_LIST_INIT_SIZE * sizeof(Array_Node));
	if(pList->node == NULL)
	{
		free(pList);
		return NULL;
	}
	//the data set null of every node
	for(i = 0;i < ARRAY_LIST_INIT_SIZE;i++)
	{
		pList->node[i].data = NULL;
	}
	pList->length = 0;
	pList->size = ARRAY_LIST_INIT_SIZE;

	//init mutexes
#ifdef MS_WINDOWS
	moon_create_32uuid(muuid);
	g_hArrayListEvent = CreateEvent(NULL, FALSE, TRUE,muuid);
	if (g_hArrayListEvent == NULL)
	{
		array_list_free(pList);
		return NULL;
	}
#endif
	return pList;
}

/**
 * function desc:
 * 		Insert the list randomly
 * params:
 *		pList:list address point.
 *		pData:the data to be inserted.
 *		index:the position to be inserted,If 0,default is inserted from the start of the list;
 			  if -1,default is inserted from the end of the list.
 * return:
 * 		Success returns 0, failure returns -1.
 */
int array_list_insert(Array_List* pList,void* pData,long index)
{
	long i = 0;
	unsigned long reallocSize = 0;//redistribute the space size.
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hArrayListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return -1;
	}
	if(index < -1 || (index > pList->length && index != -1))
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return -1;
	}
	//if the pre-allocated list space is used up,and it will be redistributed.
	if(pList->length == pList->size - 1)
	{
		//redistribute space
		reallocSize = pList->size + ARRAY_LIST_INCREASE_SIZE;
		pList->node = (Array_Node*)realloc(pList->node,reallocSize * sizeof(Array_Node));
		if(pList->node == NULL)
		{
#ifdef MS_WINDOWS
			SetEvent(g_hArrayListEvent);
#endif
			return -1;
		}
		//set data is NULL of the new node
		for(i = pList->length;i < reallocSize;i++)
		{
			pList->node[i].data = NULL;
		}
		pList->size = reallocSize;
	}
	//start insert data
	if(index == -1)//insert from end
	{
		pList->node[pList->length].data = pData;
		pList->length++;//length self-increasing 1 of list
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return 0;
	}
	else if(index == 0) //insert from start
	{
		for(i = pList->length;i >0;i--)
		{
			pList->node[i] = pList->node[i - 1];
		}
		pList->node[0].data = pData;
		pList->length++;//length self-increasing 1 of list
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return 0;
	}
	else//the specified location is inserted.
	{
		for(i = pList->length;i > index;i--)
		{
			pList->node[i] = pList->node[i - 1];
		}
		pList->node[i].data = pData;
		pList->length++;//length self-increasing 1 of list
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
	return 0;
}

/**
 * function desc:
 * 		deletes the element by index.the deleted element simply sets the data domain to NULL,
 * 		the data pointer is not released,it released by the caller，or creator.
 * params:
 *		pList:the adress of list
 *		index:location index
 */
void array_list_removeAt(Array_List* pList,unsigned long index)
{
	int i = 0;
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hArrayListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return;
	}
	if(index < 0 || index >= pList->length)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return;
	}
	for(i = index; i < pList->length;i++)//after deleting the element，let the rest of the elements move forward.
	{
		pList->node[i] = pList->node[i + 1];
	}
	//NULL to be set the data domain of the last element.
	pList->node[pList->length - 1].data = NULL;
	//length minus 1
	pList->length--;
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
}

/**
 * function desc:
 * 		delete one element
 * param:
 *     pList:the address of list
 *	   pData:element address point
 */
void array_list_remove(Array_List* pList,void* pData)
{
	int i = 0;
	int removeIndex = -1;//the index of to be delete element
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hArrayListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
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
		{
#ifdef MS_WINDOWS
			SetEvent(g_hArrayListEvent);
#endif
			return;
		}
		for(i = removeIndex; i < pList->length;i++)//after deleting the element，let the rest of the elements move forward.
		{
			pList->node[i] = pList->node[i + 1];
		}
		//NULL to be set the data domain of the last element.
		pList->node[pList->length - 1].data = NULL;
		//length minus 1
		pList->length--;
	}
	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
	SetEvent(g_hArrayListEvent);
#endif
}

/**
 * function desc:
 * 		extract the element by index.
 * params:
 *		pList:the address of list
 *		index:location index
 */
void* array_list_getAt(Array_List* pList,unsigned long index)
{
	void* pData = NULL;
	if(pList == NULL)
	{
		return NULL;
	}
	if(index < 0 || index >= pList->length)
	{
		return NULL;
	}
	pData = pList->node[index].data;
	return pData;
}

/**
 * function desc:
 * 		To empty the list
 * params:
 *	pList:the address of list
 */
void array_list_clear(Array_List* pList)
{
	int i = 0;
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hArrayListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return;
	}
	//NULL to be set the data domain 
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
	SetEvent(g_hArrayListEvent);
#endif
}

/**
 * function desc:
 * 		release list
 * params:
 *	pList:the address of list
 */
void array_list_free(Array_List* pList)
{
//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hArrayListEvent, INFINITE);
#endif
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hArrayListEvent);
#endif
		return;
	}
	//release node
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
#ifdef MS_WINDOWS
	SetEvent(g_hArrayListEvent);
	if (g_hArrayListEvent != NULL)
	{
		CloseHandle(g_hArrayListEvent);
		g_hArrayListEvent = NULL;
	}
#endif
}

/**
 * function desc:
 * 		Array_List test
 */
void array_list_test()
{
	int i = 0;
	int *p = NULL;
	Array_List* list = array_list_init();
	for(i = 0;i < 1000;i++)
	{
		p = (int*) malloc(sizeof(int));
		*p = i;
		array_list_insert(list,p,-1);
	}
	p = (int*)array_list_getAt(list,499);
	array_list_removeAt(list,499);
	free(p);
	p = (int*)array_list_getAt(list,499);
	for(i = 0;i<list->length;i++)
	{
		p = (int*)array_list_getAt(list,i);
		free(p);
	}
	array_list_clear(list);
	array_list_free(list);
}
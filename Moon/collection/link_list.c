#include "link_list.h"
#include <stdlib.h>
#include <malloc.h>

#include "../module/moon_string.h"

#ifdef _MSC_VER/* only support win32 and greater. */
#include <windows.h>
#define MS_WINDOWS
static HANDLE g_hLinkListEvent;
#endif

/**
 * function desc:
 * 		init list
 * return:
 * 		if success return the list address，and return NULL if failed
 */
Link_List* link_list_init()
{
	moon_char muuid[50] = {0};
	Link_List* pList = NULL;
	pList = (Link_List*)malloc(sizeof(Link_List));
	if(pList == NULL)
	{
		return NULL;
	}
	pList->length = 0;
	pList->head = NULL;
	pList->trail = NULL;
	//init mutexes

#ifdef MS_WINDOWS
	moon_create_32uuid(muuid);
	g_hLinkListEvent = CreateEvent(NULL, FALSE, TRUE, muuid);
	if (g_hLinkListEvent == NULL)
	{
		link_list_free(pList);
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
int link_list_insert(Link_List* pList,void* pData,long index)
{
	long i = 0;
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hLinkListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return -1;
	}
	if(index < -1 || (index > pList->length && index != -1))
	{
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return -1;
	}
	//whether to insert for the first time.
	if(pList->length == 0)
	{
		Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
		if(pNode == NULL)
		{
#ifdef MS_WINDOWS
			SetEvent(g_hLinkListEvent);
#endif
			return -1;
		}
		pNode->data = pData;
		pNode->priorNode = NULL;
		pNode->nextNode = NULL;
		pList->head = pNode;
		pList->trail = pNode;
		pList->length++;
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return 0;
	}
	else
	{
		if(-1 == index)//insert from end
		{
			//create node
			Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
			if(pNode == NULL)
			{
#ifdef MS_WINDOWS
				SetEvent(g_hLinkListEvent);
#endif
				return -1;
			}
			pNode->data = pData;
			pNode->nextNode = NULL;
			pNode->priorNode = pList->trail;
			//append the node to the end.
			pList->trail->nextNode = pNode;
			pList->trail = pNode;
			pList->length++;
		}
		else if(0 == index) //insert from start
		{
			//create node
			Link_Node* pNode = (Link_Node*)malloc(sizeof(Link_Node));
			if(pNode == NULL)
			{
#ifdef MS_WINDOWS
				SetEvent(g_hLinkListEvent);
#endif
				return -1;
			}
			pNode->data = pData;
			pNode->nextNode = pList->head;
			pNode->priorNode = NULL;
			//append the node to the head
			pList->head->priorNode = pNode;
			pList->head = pNode;
			pList->length++;
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
			return 0;
		}
		else//insert by specified index
		{
			Link_Node* pNode = pList->head;
			i=0;
			while(pNode != NULL)
			{
				if(index == i)//find the index that to be insert
				{
					//create node
					Link_Node* pCurrentNode = (Link_Node*)malloc(sizeof(Link_Node));
					if(pCurrentNode == NULL)
					{
#ifdef MS_WINDOWS
						SetEvent(g_hLinkListEvent);
#endif
						return -1;
					}
					pCurrentNode->nextNode = pNode;
					pCurrentNode->priorNode = pNode->priorNode;
					pNode->priorNode->nextNode = pCurrentNode;
					pNode->priorNode = pCurrentNode;
					pList->length++;
#ifdef MS_WINDOWS
					SetEvent(g_hLinkListEvent);
#endif
					return 0;
				}
				pNode = pNode->nextNode;
				i++;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
	return 0;
}

/**
 * function desc:
 * 		extract the element by index.
 * params:
 *		pList:the address of list
 *		index:location index
 */
void* link_list_getAt(Link_List* pList,unsigned long index)
{
	int i = 0;
	Link_Node* pNode = NULL;
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
	return NULL;
}

/**
 * function desc:
 * 		deletes the element by index.the deleted element simply sets the data domain to NULL,
 * 		the data pointer is not released,it released by the caller，or creator.
 * params:
 *		pList:the adress of list
 *		index:location index
 */
void link_list_removeAt(Link_List* pList,unsigned long index)
{
	int i = 0;
	Link_Node* pNode = NULL;
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hLinkListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return;
	}
	i = 0;
	pNode = pList->head;
	while(pNode != NULL)
	{
		if(i == index)
		{
			//find node
			if(pNode->priorNode == NULL) //head node
			{
				Link_Node* head = pList->head;
				pList->head = head->nextNode;
				pList->head->priorNode = NULL;
				free(head);//release node
				pList->length--;
			}
			else if(pNode->nextNode == NULL)//trial node
			{
				Link_Node* trial = pList->trail;
				pList->trail = trial->priorNode;
				pList->trail->nextNode = NULL;
				free(trial);//release node
				pList->length--;
			}
			else
			{
				pNode->priorNode->nextNode = pNode->nextNode;
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
	SetEvent(g_hLinkListEvent);
#endif
}

/**
 * function desc:
 * 		To empty the list
 * params:
 *	pList:the address of list
 */
void link_list_clear(Link_List* pList)
{
	Link_Node* pNode = NULL;
	//thread synchronization under windows platform
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hLinkListEvent, INFINITE);
#endif
	//////////////////////////////////////////////////////////////////////////
	//critical region
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return;
	}
	//release from trail
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
		SetEvent(g_hLinkListEvent);
#endif
}

/**
 * function desc:
 * 		release list
 * params:
 *	pList:the address of list
 */
void link_list_free(Link_List* pList)
{
	Link_Node* pNode = NULL;
#ifdef MS_WINDOWS
	WaitForSingleObject(g_hLinkListEvent, INFINITE);
#endif
	if(pList == NULL)
	{
#ifdef MS_WINDOWS
		SetEvent(g_hLinkListEvent);
#endif
		return;
	}
	//release from trail
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
	SetEvent(g_hLinkListEvent);
	if (g_hLinkListEvent != NULL)
	{
		CloseHandle(g_hLinkListEvent);
		g_hLinkListEvent = NULL;
	}
#endif
}


/**
 * Link_List test
 */
void link_list_test()
{
	int i = 0;
	int *p = NULL;
	Link_List* list = link_list_init();
	for(i = 0;i < 1000;i++)
	{
		p = (int*) malloc(sizeof(int));
		*p = i;
		link_list_insert(list,p,-1);
	}
	p = (int*)link_list_getAt(list,499);
	link_list_removeAt(list,499);
	free(p);
	p = (int*)link_list_getAt(list,499);
	for(i = 0;i<list->length;i++)
	{
		p = (int*)link_list_getAt(list,i);
		free(p);
	}
	link_list_clear(list);
	link_list_free(list);
}
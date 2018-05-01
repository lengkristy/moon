/**************************************************************************
 * This file is implemented as a link list.
 * coding by: haoran dai
 * time:2017-8-2
 **************************************************************************/
#pragma once
#ifndef _Link_LIST_H
#define _Link_LIST_H
#ifdef __cplusplus
extern "C" {
#endif

//define node,Two-way linked list
typedef struct _Link_Node{
	void* data;//data field
	struct _Link_Node* priorNode;//the pointer of point prior node
	struct _Link_Node* nextNode;//the pointer of next prior node
}Link_Node;

//define the struct of link list
typedef struct _Link_List{
	Link_Node *head;//the head pointer of link list
	Link_Node *trail;//the trail pointer of link list
	unsigned long length;//the current storage length of link list
}Link_List;

/**
 * function desc:
 * 		init list
 * return:
 * 		if success return the list address，and return NULL if failed
 */
Link_List* link_list_init();

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
int link_list_insert(Link_List* pList,void* pData,long index);

/**
 * function desc:
 * 		extract the element by index.
 * params:
 *		pList:the address of list
 *		index:location index
 */
void* link_list_getAt(Link_List* pList,unsigned long index);

/**
 * function desc:
 * 		deletes the element by index.the deleted element simply sets the data domain to NULL,
 * 		the data pointer is not released,it released by the caller，or creator.
 * params:
 *		pList:the adress of list
 *		index:location index
 */
void link_list_removeAt(Link_List* pList,unsigned long index);


/**
 * function desc:
 * 		To empty the list
 * params:
 *	pList:the address of list
 */
void link_list_clear(Link_List* pList);

/**
 * function desc:
 * 		release list
 * params:
 *	pList:the address of list
 */
void link_list_free(Link_List* pList);

/**
 * Link_List test
 */
void link_list_test();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
#endif
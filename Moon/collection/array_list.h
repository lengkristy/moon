/**************************************************************************
 * This file is implemented as a array list.
 * coding by: haoran dai
 * time:2017-8-2
 **************************************************************************/
#pragma once
#ifndef _ARRAY_LIST_H
#define _ARRAY_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

//the initial size of array list
#define ARRAY_LIST_INIT_SIZE 100
//the size of each increment when list is filled
#define ARRAY_LIST_INCREASE_SIZE 200

//define node
typedef struct _Array_Node{
	void* data;//data filed
}Array_Node;

//the struct of array list
typedef struct _Array_List{
	Array_Node *node;//node
	unsigned long length;//the current storage length of array list
	unsigned long size;//the overall size of array list,
}Array_List;

/**
 * function desc:
 * 		init list
 * return:
 * 		if success return the list address，and return NULL if failed
 */
Array_List* array_list_init();

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
int array_list_insert(Array_List* pList,void* pData,long index);

/**
 * function desc:
 * 		extract the element by index.
 * params:
 *		pList:the address of list
 *		index:location index
 */
void* array_list_getAt(Array_List* pList,unsigned long index);

/**
 * function desc:
 * 		deletes the element by index.the deleted element simply sets the data domain to NULL,
 * 		the data pointer is not released,it released by the caller，or creator.
 * params:
 *		pList:the adress of list
 *		index:location index
 */
void array_list_removeAt(Array_List* pList,unsigned long index);

/**
 * function desc:
 * 		delete one element
 * param:
 *     pList:the address of list
 *	   pData:element address point
 */
void array_list_remove(Array_List* pList,void* pData);

/**
 * function desc:
 * 		To empty the list
 * params:
 *	pList:the address of list
 */
void array_list_clear(Array_List* pList);

/**
 * function desc:
 * 		release list
 * params:
 *	pList:the address of list
 */
void array_list_free(Array_List* pList);

/**
 * function desc:
 * 		Array_List test
 */
void array_list_test();

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif

#endif

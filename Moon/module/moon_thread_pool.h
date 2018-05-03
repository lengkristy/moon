/***********************************************************************
 *  this file is thread pool                                             
 *  coding by haoran dai
 *  time 2018-4-30 11:04
************************************************************************/
#pragma once
#ifndef _MOON_THREAD_POOL_H
#define _MOON_THREAD_POOL_H
#include "../cfg/environment.h"
#ifdef MS_WINDOWS
#include <Windows.h>
#endif
#include "../collection/array_list.h"

#ifdef __cplusplus
extern "C" {
#endif

	/************************************************************************/
	/* define thread                                                        */
	/************************************************************************/
	typedef struct _Moon_Thread{
#ifdef MS_WINDOWS
		HANDLE m_handle;//windows thread handle
		DWORD m_dThreadId;//win32 thread id
#endif
	}Moon_Thread;
	
	/**
	 * function desc:
	 * 		create thread pool
	 * params:
	 * 		size:thread pool size
	 * 		pFunc:thread function
	 * return:
	 * 		success return thread pool address,failed return NULL
	 */
	Array_List* moon_create_thread_pool(int size,
#ifdef MS_WINDOWS
		LPTHREAD_START_ROUTINE pFunc,
#endif
		void* param
		);

	/**
	 * function desc:
	 * 		Gets the available thread.
	 * params:
	 * 		pThread:thread pool address
	 * return:
	 * 		success return thread struct,failed return NULL
	 */
	Moon_Thread moon_get_usable_thread(Array_List* pThread);
	
	/**
	 * function desc:
	 * 		release thread pool
	 * params:
	 * 		pThread:thread pool address
	 */
	void moon_destory_thread_pool(Array_List* pThread);
#ifdef __cplusplus
}
#endif

#endif

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
	
	/************************************************************************/
	/* 创建线程池                                                           */
	/*    参数:size   线程池大小											*/
	/*		   pFunc  线程开始函数											*/
	/*	  返回值：创建成功返回线程数组地址，创建失败返回NULL    			*/
	/************************************************************************/
	Array_List* moon_create_thread_pool(int size,
#ifdef MS_WINDOWS
		LPTHREAD_START_ROUTINE pFunc,
#endif
		void* param
		);

	/************************************************************************/
	/* 获取可使用的线程                                                     */
	/*	参数：pThread  线程池的指针											*/
	/*	返回值：成功返回一个可用的线程结构，失败返回NULL					*/
	/************************************************************************/
	Moon_Thread moon_get_usable_thread(Array_List* pThread);

	/************************************************************************/
	/* 释放线程池                                                           */
	/*	参数：pThread  线程池指针											*/
	/************************************************************************/
	void moon_destory_thread_pool(Array_List* pThread);
#ifdef __cplusplus
}
#endif

#endif

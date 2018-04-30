#include "../cfg/environment.h"
#include "moon_thread_pool.h"
#include "moon_config_struct.h"
#include "moon_malloc.h"
#ifdef MS_WINDOWS
#include <Windows.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif

	extern Moon_Server_Config* p_global_server_config;//服务配置全局变量

	/************************************************************************/
	/* 创建线程池                                                           */
	/*    参数:size   线程池大小											*/
	/*	  返回值：创建成功返回线程数组地址，创建失败返回NULL    			*/
	/************************************************************************/
	Array_List* moon_create_thread_pool(int size,
#ifdef MS_WINDOWS
		LPTHREAD_START_ROUTINE pFunc,
#endif
		void* param
		)
	{
#ifdef MS_WINDOWS
		HANDLE handle = NULL;
		DWORD threadId = 0;
#endif
		int i = 0;
		Array_List* pThreads = array_list_init();
		if (pThreads == NULL)
		{
			return NULL;
		}
		for ( i = 0 ; i < size ;i++)
		{
#ifdef MS_WINDOWS
			handle = CreateThread(NULL,0,pFunc,(LPVOID)param,CREATE_SUSPENDED,&threadId);
			if (handle != NULL)
			{
				Moon_Thread* pThread = (Moon_Thread*)moon_malloc(sizeof(Moon_Thread));
				pThread->m_handle = handle;
				pThread->m_dThreadId = threadId;
				if (array_list_insert(pThreads,pThread,0) == -1)
				{
					return NULL;
				}
			}
#endif
		}
		return pThreads;
	}

#ifdef __cplusplus
}
#endif
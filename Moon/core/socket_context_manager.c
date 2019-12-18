#include "../collection/array_list.h"
#include "../cfg/environment.h"
#include "ms_socket_context.h"

#ifdef __cplusplus
extern "C" {
#endif

Array_List* g_pClientSocketContext = NULL;//the context of client socket

/**
 * 函数说明：
 *	初始化socket上下文管理器
 */
void init_socket_context_manager()
{
	if(g_pClientSocketContext == NULL)
	{
		g_pClientSocketContext = array_list_init();
	}
}

/**
 * 函数说明：
 *	添加客户端socket上下文
 * 参数：
 *	socket上下文的结构体指针，如果是windows的话，那么就是：MS_SOCKET_CONTEXT *
 */
void add_socket_context(void* psocket_context)
{
	array_list_insert(g_pClientSocketContext,psocket_context,-1);
}

/**
 * 函数说明：
 *	删除客户端socket上下文
 * 参数：
 *	socket上下文的结构体指针，如果是windows的话，那么就是：MS_SOCKET_CONTEXT *
 */
void remove_socket_context(void* psocket_context)
{
	array_list_remove(g_pClientSocketContext,psocket_context);
#ifdef MS_WINDOWS
	free_socket_context((MS_SOCKET_CONTEXT *)psocket_context);
#endif
}

#ifdef MS_WINDOWS
/**
 * 函数说明：
 *   通过客户端id查找socket context
 * 参数：
 *   client_id：客户端id
 * 返回值：
 *   如果查询到则返回socket context指针，没有查询到返回NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_client_id(moon_char* client_id)
{
	int index = 0;
	MS_SOCKET_CONTEXT* pSocketContext = NULL;
	for (index = 0;index < g_pClientSocketContext->length;index++)
	{
		pSocketContext = (MS_SOCKET_CONTEXT*)array_list_getAt(g_pClientSocketContext,index);
		if(moon_char_equals(client_id,pSocketContext->m_client_id))
		{
			return pSocketContext;
			break;
		}
	}
	return NULL;
}
#endif

/**
 * 函数说明：
 *   获取当前所有客户端数量
 * 返回值：
 *   返回数量
 */
int get_socket_context_count()
{
	if(g_pClientSocketContext != NULL)
	{
		return g_pClientSocketContext->length;
	}
	return 0;
}


#ifdef MS_WINDOWS
/**
 * 函数说明：
 *   通过索引获取socket context
 * 参数：
 *   index：索引
 * 返回值：
 *   如果查询到则返回socket context指针，没有查询到返回NULL
 */
MS_SOCKET_CONTEXT * get_socket_context_by_index(int index)
{
	if(index < 0 || index > g_pClientSocketContext->length)
	{
		return NULL;
	}
	return (MS_SOCKET_CONTEXT *)array_list_getAt(g_pClientSocketContext,index);
}
#endif


#ifdef __cplusplus
}
#endif
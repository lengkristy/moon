#include "../collection/array_list.h"
#include "../cfg/environment.h"
#include "../module/moon_base.h"
#include "moon_session_manager.h"
#include "../module/moon_memory_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

Array_List* g_pmoon_sessions = NULL;//the context of client socket

/**
 * 函数说明：
 *	初始化socket上下文管理器
 */
void init_session_manager()
{
	if(g_pmoon_sessions == NULL)
	{
		g_pmoon_sessions = array_list_init();
	}
}

/**
 * 函数说明：
 *	添加客户端session
 * 参数：
 *	p_moon_session：客户端session指针
 */
void add_session(moon_session* p_moon_session)
{
	if(p_moon_session == NULL) return;
	array_list_insert(g_pmoon_sessions,p_moon_session,-1);
}

/**
 * 函数说明：
 *	删除客户端session
 * 参数：
 *	p_moon_session：客户端session指针
 */
void remove_session(moon_session* p_moon_session)
{
	if(p_moon_session == NULL) return;
	array_list_remove(g_pmoon_sessions,p_moon_session);
	if(p_moon_session->p_client_environment != NULL)
	{
		moon_free(p_moon_session->p_client_environment);
	}
	if(p_moon_session->p_socket_context != NULL)
	{
#ifdef MS_WINDOWS
		free_socket_context(p_moon_session->p_socket_context);
#endif
	}
	moon_free(p_moon_session);
}

/**
 * 函数说明：
 *   通过客户端id查找socket context
 * 参数：
 *   client_id：客户端id
 * 返回值：
 *   返回客户端session指针，如果失败返回NULL
 */
moon_session* get_moon_session_by_client_id(moon_char* client_id)
{
	int index = 0;
	moon_session* p_moon_session = NULL;
	for (index = 0;index < g_pmoon_sessions->length;index++)
	{
		p_moon_session = (moon_session*)array_list_getAt(g_pmoon_sessions,index);
		if(p_moon_session != NULL && p_moon_session->p_client_environment->client_id != NULL && 
			moon_char_equals(client_id,p_moon_session->p_client_environment->client_id))
		{
			return p_moon_session;
			break;
		}
	}
	return NULL;
}

/**
 * 函数说明：
 *   获取当前所有客户端数量
 * 返回值：
 *   返回数量
 */
int get_socket_context_count()
{
	if(g_pmoon_sessions != NULL)
	{
		return g_pmoon_sessions->length;
	}
	return 0;
}


/**
 * 函数说明：
 *   通过索引获取socket context
 * 参数：
 *   index：索引
 * 返回值：
 *   返回客户端session指针，如果失败返回NULL
 */
moon_session* get_moon_session_by_index(int index)
{
	if(index < 0 || index > g_pmoon_sessions->length)
	{
		return NULL;
	}
	return (moon_session*)array_list_getAt(g_pmoon_sessions,index);
}


#ifdef __cplusplus
}
#endif